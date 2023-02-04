#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sampler.h"

void *myfunc()
{
    Sampler_startSampling();
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t myThread;
    pthread_create(&myThread, NULL, myfunc, NULL);
    
    pthread_join(myThread, NULL);
    return 0;
}