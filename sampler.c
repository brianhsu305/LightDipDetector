#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "circular_buffer.h"

#define A2D_FILE_VOLTAGE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

int numOfSamples = 0;
int bufferSize = 1000;
circular_buffer lightSampleBuffer;

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

int getVoltage1Reading(void)
{
    // Open file
    FILE *f = fopen(A2D_FILE_VOLTAGE1, "r");
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
    // while(1) {
    buffer_init(&lightSampleBuffer, bufferSize, sizeof(int));

    for (int i = 0; i < 1000; i++)
    {
        int reading = getVoltage1Reading();
        // double voltage = ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
        numOfSamples += 1;
        // printf("Value %5d ==> %5.2fV\n", reading, voltage);
        sleepForMs(1);

        // add data into buffer
        buffer_AddData(&lightSampleBuffer, &reading);
        printf("data: %d\n", lightSampleBuffer.buffer[i]);
    }
    free(lightSampleBuffer.buffer);
    return;
}

void Sampler_stopSampling(void)
{
    return;
}

// Set/get the maximum number of samples to store in the history.
void Sampler_setHistorySize(int newSize)
{
    lightSampleBuffer.length = newSize;
    return;
}

int Sampler_getHistorySize(void)
{
    return lightSampleBuffer.count;
}

// Get a copy of the samples in the sample history.
// Returns a newly allocated array and sets `length` to be the
// number of elements in the returned array (output-only parameter). // The calling code must call free() on the returned pointer.
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
