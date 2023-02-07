#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "sampler.h"
#include "segDisplay.h"
#include "udp_sockets.h"

int main(int argc, char *argv[])
{
    // create threads
    Sampler_samplingThreadInit();
    Sampler_printThreadInit();
    segDisplay_threadInit();
    UDP_threadInit();
    return 0;
}