#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include "sampler.h"

#define MAX_LEN 1024
#define PORT 12345

pthread_t UDP_thread;
struct sockaddr_in mySocket, sinRemote;
unsigned int sin_len = sizeof(sinRemote);
char messageRx[MAX_LEN];

int socketDescriptor;
int bytesRx;
bool readingData = true;;

void UDP_serverInit()
{
    memset(&mySocket, 0, sizeof(mySocket));
    mySocket.sin_family = AF_INET;
    mySocket.sin_addr.s_addr = htonl(INADDR_ANY);
    mySocket.sin_port = htons(PORT);
    socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
    bind(socketDescriptor, (struct sockaddr *)&mySocket, sizeof(mySocket));
    printf("UDP is working\n");
}
void UDP_receiveData()
{
    bytesRx = recvfrom(socketDescriptor, messageRx, MAX_LEN - 1, 0, (struct sockaddr *)&sinRemote, &sin_len);
    printf("Message received (%d bytes): '%s'\n", bytesRx, messageRx);
}

void UDP_reply() {
    // Create reply
    char messageTx[MAX_LEN];
    snprintf(messageTx, MAX_LEN, "Hello %d\n", 42);

    // Send reply
    sin_len = sizeof(sinRemote);
    sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
}

void *UDP_threadFunc()
{
    UDP_serverInit();
    while (readingData) {
        UDP_receiveData();
        UDP_reply();
    }
    close(socketDescriptor);
    return NULL;
}

void UDP_threadInit(void)
{
    pthread_create(&UDP_thread, NULL, UDP_threadFunc, NULL);
    pthread_join(UDP_thread, NULL);
}
