#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#include "sampler.h"
#include "circular_buffer.h"
#include "periodTimer.h"
#include "segDisplay.h"
#include "udp_sockets.h"

#define A2D_FILE_VOLTAGE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095
#define EXPONENTIAL_WEIGHT_VALUE 0.001
#define I2C_RIGHT_DIGIT "/sys/class/gpio/gpio44/value"
#define I2C_LEFT_DIGIT "/sys/class/gpio/gpio61/value"

Period_statistics_t pStats;
pthread_t lightSampleThread, printDataThread, segDisplayThread;
static pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
static long long startingBufferSize = 1000;
circular_buffer lightSampleBuffer;
static bool readingData = true, printingData = true;
static int potValue;
static int historySize;
static double sampleLightLevel;
static double avgLightLevel = 0;
static int dipCount = 0;
static double PrevAveLightLevel;
static bool dipDetectReady = false;

void sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *)NULL);
}

int getVoltageReading(char *a2dFileVoltagePath)
{
    // Open file
    FILE *f = fopen(a2dFileVoltagePath, "r");
    if (!f)
    {
        printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
        printf(" Check /boot/uEnv.txt for correct options.\n");
        exit(-1);
    }
    // Get reading
    int a2dReading = 0;
    int itemsRead = fscanf(f, "%d", &a2dReading);
    if (itemsRead <= 0)
    {
        printf("ERROR: Unable to read values from voltage input file.\n");
        exit(-1);
    }
    // Close file
    fclose(f);
    return a2dReading;
}

// Get the average light level (not tied to the history).
double Sampler_getAverageReading(void)
{
    // the average has not been calculated
    if (avgLightLevel == 0)
    {
        avgLightLevel = lightSampleBuffer.buffer[lightSampleBuffer.tail];
    }
    else
    {
        // Weights the previous average at 99.9%
        avgLightLevel = (EXPONENTIAL_WEIGHT_VALUE * lightSampleBuffer.buffer[lightSampleBuffer.tail]) + ((1 - EXPONENTIAL_WEIGHT_VALUE) * avgLightLevel);
    }

    // increment tail index after reading the data
    if (lightSampleBuffer.head % lightSampleBuffer.size == 0)
    {
        lightSampleBuffer.tail = 0;
    }
    else
    {
        lightSampleBuffer.tail++;
    }

    return avgLightLevel;
}

// Get a copy of the samples in the sample history.
// Returns a newly allocated array and sets `length` to be the
// number of elements in the returned array (output-only parameter).
// The calling code must call free() on the returned pointer.
// Note: provides both data and size to ensure consistency.
double *Sampler_getHistory(int *length)
{
    double *copySample;
    copySample = malloc(sizeof(double) * (*length + 1));
    pthread_mutex_lock(&myMutex);
    for (int i = 0; i < *length; i++)
    {
        copySample[i] = lightSampleBuffer.buffer[i];
    }
    pthread_mutex_unlock(&myMutex);
    return copySample;
}

void Sampler_dipDetection()
{
    // When detected light is lower then average by 0.1V
    if (dipDetectReady)
    {
        if (avgLightLevel - sampleLightLevel > 0.1)
        {
            dipCount++;
            PrevAveLightLevel = avgLightLevel;
            dipDetectReady = false;
        }
    }
    else
    {
        if (sampleLightLevel - avgLightLevel > 0.03)
        {
            dipDetectReady = true;
        }
    }
}

// Begin/end the background thread which samples light levels.
void Sampler_startSampling(void)
{
    buffer_init(&lightSampleBuffer, startingBufferSize);

    while (readingData)
    {

        // get the light sample reading and calculate voltage
        int voltageReading = getVoltageReading(A2D_FILE_VOLTAGE1);
        Period_markEvent(PERIOD_EVENT_SAMPLE_LIGHT);
        sampleLightLevel = ((double)voltageReading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
        // add voltage sample into buffer
        pthread_mutex_lock(&myMutex);
        {
            buffer_AddData(&lightSampleBuffer, sampleLightLevel);
            avgLightLevel = Sampler_getAverageReading();
            Sampler_dipDetection();
        }
        pthread_mutex_unlock(&myMutex);
        sleepForMs(1);
    }
    return;
}

void Sampler_stopSampling(void)
{
    Period_cleanup();
    readingData = false;
    printingData = false;

    buffer_free(&lightSampleBuffer);
    pthread_join(lightSampleThread, NULL);
    pthread_join(printDataThread, NULL);
    pthread_join(segDisplayThread, NULL);
    exit(1);
    return;
}

// Set the maximum number of samples to store in the history.
void Sampler_setHistorySize(int newSize)
{
    // // set the buffer length to newSize
    // lightSampleBuffer.size = newSize;
    // return;
    double *tempBuffer = malloc(sizeof(double) * (newSize + 1));
    pthread_mutex_lock(&myMutex);
    {
        lightSampleBuffer.head = lightSampleBuffer.size;
        lightSampleBuffer.tail = lightSampleBuffer.size;
        // if then new size > the original size
        if (newSize >= lightSampleBuffer.size)
        {
            int numOfSamples = lightSampleBuffer.size;
            if (numOfSamples >= lightSampleBuffer.count)
            {
                numOfSamples = lightSampleBuffer.count;
            }

            for (int i = 0; i < numOfSamples; i++)
            {
                tempBuffer[i] = lightSampleBuffer.buffer[i];
            }
        }
        // if the new size <= the original size
        else
        {
            for (int i = 0; i < newSize; i++)
            {
                tempBuffer[i] = lightSampleBuffer.buffer[i + lightSampleBuffer.size - newSize];
            }
            lightSampleBuffer.head = 0;
            lightSampleBuffer.tail = 0;
        }
        lightSampleBuffer.size = newSize;

        // Copy the new buffer data to the circular buffer struct
        free(lightSampleBuffer.buffer);
        lightSampleBuffer.buffer = tempBuffer;
    }
    pthread_mutex_unlock(&myMutex);
    return;
}

// Get the maximum number of samples to store in the history
int Sampler_getHistorySize(void)
{
    potValue = getVoltageReading(A2D_FILE_VOLTAGE0);
    if (potValue == 0)
    {
        return 1;
    }
    return potValue;
}

// Returns how many valid samples are currently in the history.
// May be less than the history size if the history is not yet full.
int Sampler_getNumSamplesInHistory()
{
    // int num = 0;
    // for (int i = 0; i < lightSampleBuffer.size; i++)
    // {
    //     if (lightSampleBuffer.buffer[i] != 0)
    //     {
    //         num++;
    //     }
    // }
    // return num;
    if (lightSampleBuffer.count >= lightSampleBuffer.size)
    {
        return lightSampleBuffer.size;
    }
    else
    {
        return lightSampleBuffer.head;
    }
}

// Get the total number of light level samples taken so far.
long long Sampler_getNumSamplesTaken(void)
{
    return lightSampleBuffer.count;
}

// Call functions in segDisplay.c to perform 2-Digit 14 Segment Display Feature
void segDisplayDip(void)
{
    Seg_init();
    while (readingData)
    {
        // If Dip Count is over 99, set the display Number as 99 (can only display up to 2 digits)
        int dipNum = dipCount;
        if (dipNum > 99)
        {
            dipNum = 99;
        }
        // separate ones and tens (Ex. 15 --> ones: 5, tens: 1)
        int dipNumOnes = dipNum % 10;
        int dipNumTens = (dipNum / 10) % 10;

        // Turn off both digits
        Seg_turnOffSegDisplay();
        // Drive Display Pattern & Turn on Left Digit
        Seg_displayDigits(dipNumTens, I2C_RIGHT_DIGIT);
        // Turn off both digits
        Seg_turnOffSegDisplay();
        // Drive Display Pattern & Turn on Right Digit
        Seg_displayDigits(dipNumOnes, I2C_LEFT_DIGIT);
    }
    return;
}

int Sampler_getDipCount()
{
    return dipCount;
}

// print relavent data
void Sampler_printData()
{
    while (printingData)
    {

        // pthread_mutex_lock(&myMutex);
        {
            Sampler_setHistorySize(Sampler_getHistorySize());
            historySize = Sampler_getNumSamplesInHistory();
        }
        // pthread_mutex_unlock(&myMutex);

        Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_LIGHT, &pStats);
        printf("Samples/s = %d Pot Value = %d history size = %d avg = %.3f dips = %d Sampling[%.3f, %.3f] avg %.3f/%d \n", pStats.numSamples, potValue, historySize, avgLightLevel, dipCount, pStats.minPeriodInMs, pStats.maxPeriodInMs, pStats.avgPeriodInMs, pStats.numSamples);

        double *history = Sampler_getHistory(&historySize);
        for (int i = 200; i < historySize; i += 200)
        {
            printf(" %.3f", history[i]);
        }
        free(history);
        printf("\n");
        sleepForMs(1000);
    }

    return;
}

void *Sampler_samplingThreadFunc(void *arg)
{
    Sampler_startSampling();
    return NULL;
}

void *Sampler_printThreadFunc(void *arg)
{
    Sampler_printData();
    return NULL;
}

void *segDisplay_threadFunc(void *arg)
{
    segDisplayDip();
    return NULL;
}

void Sampler_threadInit()
{
    Period_init();
    pthread_create(&lightSampleThread, NULL, Sampler_samplingThreadFunc, NULL);
    pthread_create(&printDataThread, NULL, Sampler_printThreadFunc, NULL);
    pthread_create(&segDisplayThread, NULL, segDisplay_threadFunc, NULL);
    UDP_threadInit();
}