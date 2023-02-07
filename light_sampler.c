#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sampler.h"

void *samplerLight()
{
    Sampler_startSampling();
    return NULL;
}

void *samplerPrintData()
{
    Sampler_printData();
    return NULL;
}

int main(int argc, char *argv[])
{
    // create threads
    pthread_t lightSampleThread;
    pthread_t printDataThread;
    pthread_create(&lightSampleThread, NULL, samplerLight, NULL);
    pthread_create(&printDataThread, NULL, samplerPrintData, NULL);

    //end program
    pthread_join(lightSampleThread, NULL);
    pthread_join(printDataThread, NULL);
    return 0;
}