#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sampler.h"

void *sampleLight()
{
    Sampler_startSampling();
    return NULL;
}

void *sampleAndPrintData()
{
    printData();
    return NULL;
}

int main(int argc, char *argv[])
{
    // create threads
    pthread_t lightSampleThread;
    pthread_t printDataThread;
    pthread_create(&lightSampleThread, NULL, sampleLight, NULL);
    pthread_create(&printDataThread, NULL, sampleAndPrintData, NULL);

    //end program
    pthread_join(lightSampleThread, NULL);
    pthread_join(printDataThread, NULL);
    return 0;
}