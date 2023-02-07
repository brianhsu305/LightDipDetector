#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "sampler.h"
#include "udp_sockets.h"

int main(int argc, char *argv[])
{
    // create threads
    Sampler_threadInit();
    UDP_threadInit();
    return 0;
}