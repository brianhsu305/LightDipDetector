#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include "sampler.h"

#define A2D_FILE_VOLTAGE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

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

    // in main() of potDriver.c
    // int reading = getVoltage1Reading();
    // double voltage = ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
    // printf("Value %5d ==> %5.2fV\n", reading, voltage); 
}

// Begin/end the background thread which samples light levels.
void Sampler_startSampling(void){
    // pthread_t sample_thread;
    // pthread_create(&sample_thread, NULL, Sampler_startSampling, NULL);
    return;
}

 void Sampler_stopSampling(void){
    return;
}

// Set/get the maximum number of samples to store in the history.
void Sampler_setHistorySize(int newSize){
    return;
}

int Sampler_getHistorySize(void){
    return 0;
}

// Get a copy of the samples in the sample history.
// Returns a newly allocated array and sets `length` to be the
// number of elements in the returned array (output-only parameter). // The calling code must call free() on the returned pointer.
// Note: provides both data and size to ensure consistency.
double* Sampler_getHistory(int *length){
    return 0;
}

// Returns how many valid samples are currently in the history.
// May be less than the history size if the history is not yet full. 
int Sampler_getNumSamplesInHistory(){
    return 0;
}


// Get the average light level (not tied to the history).
double Sampler_getAverageReading(void){
    return 0;
}

// Get the total number of light level samples taken so far.
long long Sampler_getNumSamplesTaken(void){
    return 0;
}
