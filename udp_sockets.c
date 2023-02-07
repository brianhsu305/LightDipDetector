#include <stdio.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include "sampler.h"

#define MAX_LEN 1024
#define PORT 12345

pthread_t UDP_thread;
struct sockaddr_in mySocket;
int socketDescriptor;
// memset(&sin, 0, sizeof(sin));

void *UDP_threadFunc()
{
    memset(&mySocket, 0, sizeof(mySocket));
    mySocket.sin_family = AF_INET;
    mySocket.sin_addr.s_addr = htonl(INADDR_ANY);
    mySocket.sin_port = htons(PORT);
    socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
    bind(socketDescriptor, (struct sockaddr *)&mySocket, sizeof(mySocket));
    printf("UDP is working\n");
    return NULL;
}

void UDP_serverInit(void)
{
    pthread_create(&UDP_thread, NULL, UDP_threadFunc, NULL);
    pthread_join(UDP_thread, NULL);
}
