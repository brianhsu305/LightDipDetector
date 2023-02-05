#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "circular_buffer.h"

#define A2D_FILE_VOLTAGE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

int numOfSamples = 0;
int bufferSize = 1000;
circular_buffer lightSampleBuffer;
bool readingData = true;
int potValue;

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

// Begin/end the background thread which samples light levels.
void Sampler_startSampling(void)
{
    buffer_init(&lightSampleBuffer, bufferSize, sizeof(int));
    while (readingData)
    // for (int i = 0; i < 1005; i++)
    {
        int voltageReading = getVoltageReading(A2D_FILE_VOLTAGE1);
        // double voltage = ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
        numOfSamples += 1;
        // printf("Value %5d ==> %5.2fV\n", reading, voltage);
        sleepForMs(1);
        // add data into buffer
        buffer_AddData(&lightSampleBuffer, &voltageReading);
        // printf("data: %d\n", lightSampleBuffer.buffer);
    }
    return;
}

void Sampler_stopSampling(void)
{
    readingData = false;
    free(lightSampleBuffer.buffer);
    return;
}

// Set the maximum number of samples to store in the history.
void Sampler_setHistorySize(int newSize)
{
    // set the buffer length to newSize
    lightSampleBuffer.length = newSize;
    return;
}
// Get the maximum number of samples to store in the history
int Sampler_getHistorySize(void)
{
    potValue = getVoltageReading(A2D_FILE_VOLTAGE0);
        if (potValue == 0)
        {
            potValue = 1;
        }
        // printf("POT reader: %d\n", potValue);
    return potValue;
}

// Get a copy of the samples in the sample history.
// Returns a newly allocated array and sets `length` to be the
// number of elements in the returned array (output-only parameter).
// The calling code must call free() on the returned pointer.
// Note: provides both data and size to ensure consistency.
double *Sampler_getHistory(int *length)
{
    return 0;
}

// Returns how many valid samples are currently in the history.
// May be less than the history size if the history is not yet full.
int Sampler_getNumSamplesInHistory()
{
    return lightSampleBuffer.count;
}

// Get the average light level (not tied to the history).
double Sampler_getAverageReading(void)
{
    return 0;
}

// Get the total number of light level samples taken so far.
long long Sampler_getNumSamplesTaken(void)
{
    return 0;
}

void printData()
{
    while (1) {
        Sampler_setHistorySize(Sampler_getHistorySize());
        // Line 1:
        // # of light samples taken during the last second 
        // the raw value read from the POT
        // # of valid samples in the history 
        // avg light level, displayed as a voltage with 3 decimal places
        // # of light level dips that have been found in sample history
        // timinig jitter information (provided bt periodTimer.h / .c)
        printf("Samples/s = %d Pot Value = %d history size = %d avg = %.3f dips = %d Sampling[%.3f, %.3f] avg%.3f/%d \n", lightSampleBuffer.count, potValue, 551, 1.307, 0, 1.403, 4.002, 1.836, 550);

        // Line 2:
        // display every 200th samples in the sample history 
        // show the oldest of these samples on the left, the newest of these samples on the right
        printf("Line 2\n");

        sleepForMs(1000);
        // HINT: use a thread 
    }
    
    return;
}