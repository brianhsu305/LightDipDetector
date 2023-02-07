#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "sampler.h"
#include "udp_sockets.h"

void *samplerPrintData()
{
    Sampler_printData();
    return NULL;
}

int main(int argc, char *argv[])
{
    // create threads
    Sampler_samplingThreadInit();
    Sampler_printThreadInit();
    UDP_threadInit();
    return 0;
}