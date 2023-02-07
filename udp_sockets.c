#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include "sampler.h"

#define MAX_LEN 2048
#define PORT 12345

pthread_t UDP_thread;
struct sockaddr_in mySocket, sinRemote;
unsigned int sin_len = sizeof(sinRemote);
char messageRx[MAX_LEN];

int socketDescriptor;
int bytesRx;
bool readingData = true;
;

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

// the received command is "help"
// reply with help comments
void UDP_replyHelp()
{
    char messageTx[MAX_LEN];
    int freeSpace;
    snprintf(messageTx, MAX_LEN, "Accepted command examples:\n");
    freeSpace = MAX_LEN - strnlen(messageTx, MAX_LEN) - 1;

    strncat(messageTx, "count  -- display total number of samples taken\n", freeSpace);
    freeSpace = freeSpace - strnlen(messageTx, freeSpace) - 1;

    strncat(messageTx, "length  -- display number of samples in history (both max, and current).\n", freeSpace);
    freeSpace = freeSpace - strnlen(messageTx, freeSpace) - 1;

    strncat(messageTx, "history  -- display the full samples history being saved.\n", freeSpace);
    freeSpace = freeSpace - strnlen(messageTx, freeSpace) - 1;

    strncat(messageTx, "get 10  -- display the 10 most recent history being saved.\n", freeSpace);
    freeSpace = freeSpace - strnlen(messageTx, freeSpace) - 1;

    strncat(messageTx, "dips  -- display number of .\n", freeSpace);
    freeSpace = freeSpace - strnlen(messageTx, freeSpace) - 1;

    strncat(messageTx, "stop  -- cause the server program to end.\n", freeSpace);
    freeSpace = freeSpace - strnlen(messageTx, freeSpace) - 1;

    strncat(messageTx, "<enter>  -- repeat last command.\n", freeSpace);
    // freeSpace = freeSpace - strnlen(messageTx, freeSpace) - 1;

    // Send reply
    sin_len = sizeof(sinRemote);
    sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
}

void UDP_replyCount()
{
    char messageTx[MAX_LEN];
    snprintf(messageTx, MAX_LEN, "Number of samples taken = %lld\n", Sampler_getNumSamplesTaken());

    // Send reply
    sin_len = sizeof(sinRemote);
    sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
}

void UDP_replyLength()
{
    char messageTx[MAX_LEN];
    int freeSpace;
    snprintf(messageTx, MAX_LEN, "History can hold %d samples\n", Sampler_getHistorySize());
    freeSpace = MAX_LEN - strnlen(messageTx, MAX_LEN) - 1;
    char buf[freeSpace];
    snprintf(buf, freeSpace, "Currently holding %d samples\n", Sampler_getNumSamplesInHistory());
    strncat(messageTx, buf, freeSpace);

    // Send reply
    sin_len = sizeof(sinRemote);
    sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
}

void UDP_replyHistory()
{
    // char messageTx[MAX_LEN];
    // char buf[MAX_LEN];
    // int freeSpace = MAX_LEN;
    // int size = Sampler_getHistorySize();
    // double *history = Sampler_getHistory(&size);
    // printf("HELOO %f\n", *history);
    // for (int i=0; i<size; i++) {
        
    //     snprintf(buf, freeSpace, "%.3f, ", history+1);
    //     strncat(messageTx, buf, freeSpace);
    //     freeSpace = freeSpace - strnlen(messageTx, MAX_LEN) - 1;
    // }
    // // Send reply
    // sin_len = sizeof(sinRemote);
    // sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
}

void UDP_replyGet()
{
    char messageTx[MAX_LEN];
    // int freeSpace;
    snprintf(messageTx, MAX_LEN, "Number of samples taken = %lld\n", Sampler_getNumSamplesTaken());
    // freeSpace = MAX_LEN - strnlen(messageTx, MAX_LEN) - 1;

    // Send reply
    sin_len = sizeof(sinRemote);
    sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
}

void UDP_replyDips()
{
    char messageTx[MAX_LEN];
    snprintf(messageTx, MAX_LEN, "Dips = %d\n", Sampler_getDipCount());

    // Send reply
    sin_len = sizeof(sinRemote);
    sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
}

void UDP_replyStop()
{
    char messageTx[MAX_LEN];
    snprintf(messageTx, MAX_LEN, "Program Terminating.\n");
    

    // Send reply
    sin_len = sizeof(sinRemote);
    sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
    close(socketDescriptor);
    Sampler_stopSampling();
}

void UDP_replyEnter()
{
    char messageTx[MAX_LEN];
    // int freeSpace;
    snprintf(messageTx, MAX_LEN, "Number of samples taken = %lld\n", Sampler_getNumSamplesTaken());
    // freeSpace = MAX_LEN - strnlen(messageTx, MAX_LEN) - 1;

    // Send reply
    sin_len = sizeof(sinRemote);
    sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
}

void UDP_receiveData()
{
    bytesRx = recvfrom(socketDescriptor, messageRx, MAX_LEN - 1, 0, (struct sockaddr *)&sinRemote, &sin_len);
    if (strncmp("help\n", messageRx, sizeof("help")) == 0)
    {
        UDP_replyHelp();
    }
    else if (strncmp("count\n", messageRx, sizeof("count")) == 0)
    {
        UDP_replyCount();
    }
    else if (strncmp("length\n", messageRx, sizeof("length")) == 0)
    {
        UDP_replyLength();
    }
    else if (strncmp("history\n", messageRx, sizeof("history")) == 0)
    {
        UDP_replyHistory();
    }
    else if (strncmp("get 10\n", messageRx, sizeof("get 10")) == 0)
    {
        UDP_replyGet();
    }
    else if (strncmp("dips\n", messageRx, sizeof("dips")) == 0)
    {
        UDP_replyDips();
    }
    else if (strncmp("stop\n", messageRx, sizeof("stop")) == 0)
    {
        UDP_replyStop();
    }
    else if (strncmp("<enter>\n", messageRx, sizeof("<enter>")) == 0)
    {
        UDP_replyEnter();
    }

    printf("Message received (%d bytes): '%s'", bytesRx, messageRx);
}

void *UDP_threadFunc()
{
    UDP_serverInit();
    while (readingData)
    {
        UDP_receiveData();
    }
    return NULL;
}

void UDP_threadInit(void)
{
    pthread_create(&UDP_thread, NULL, UDP_threadFunc, NULL);
    pthread_join(UDP_thread, NULL);
}