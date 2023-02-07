#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "sampler.h"
#include "udp_sockets.h"

#define MAX_LEN 2048
#define PORT 12345

pthread_t UDP_thread;
struct sockaddr_in mySocket, sinRemote;
unsigned int sin_len = sizeof(sinRemote);
char messageRx[MAX_LEN];
char lastCommand[MAX_LEN];

int socketDescriptor;
int bytesRx;
bool readingData = true;

void UDP_serverInit()
{
    memset(&mySocket, 0, sizeof(mySocket));
    mySocket.sin_family = AF_INET;
    mySocket.sin_addr.s_addr = htonl(INADDR_ANY);
    mySocket.sin_port = htons(PORT);
    socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
    bind(socketDescriptor, (struct sockaddr *)&mySocket, sizeof(mySocket));
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
    char messageTx[MAX_LEN];
    char buf[MAX_LEN];
    int freeSpace = MAX_LEN;
    int size = Sampler_getNumSamplesInHistory();
    double *history = Sampler_getHistory(&size);
    for (int i = 0; i < size; i++)
    {

        snprintf(buf, freeSpace, "%.3f, ", history[i]);
        strncat(messageTx, buf, freeSpace);
        freeSpace = freeSpace - strnlen(messageTx, MAX_LEN) - 1;

        if (freeSpace < 0)
        {
            // Send reply
            sin_len = sizeof(sinRemote);
            sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
            messageTx[0] = '\0';
            buf[0] = '\0';
            freeSpace = MAX_LEN;
        }
    }
    // Send reply
    sin_len = sizeof(sinRemote);
    strncat(messageTx, "\n", freeSpace);
    free(history);
    sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
}

void UDP_replyGet(int num)
{
    char messageTx[MAX_LEN];
    // snprintf(messageTx, MAX_LEN, "INSIDE GET  = %d\n", num);
    char buf[MAX_LEN];
    int freeSpace = MAX_LEN;
    int size = Sampler_getNumSamplesInHistory();
    double *history = Sampler_getHistory(&size);
    if (num > size) {
        snprintf(messageTx, freeSpace, "input size is bigger then history size\n");
        sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
        return;
    }
    for (int i = 0; i < num; i++)
    {

        snprintf(buf, freeSpace, "%.3f, ", history[size - i - 1]);
        strncat(messageTx, buf, freeSpace);
        freeSpace = freeSpace - strnlen(messageTx, MAX_LEN) - 1;

        if (freeSpace < 0)
        {
            // Send reply
            sin_len = sizeof(sinRemote);
            sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
            messageTx[0] = '\0';
            buf[0] = '\0';
            freeSpace = MAX_LEN;
        }
    }

    // Send reply
    sin_len = sizeof(sinRemote);
    strncat(messageTx, "\n", freeSpace);
    free(history);
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
    // copy the last command
    snprintf(messageRx, MAX_LEN, lastCommand);
    UDP_readCommand(lastCommand);

    // // Send reply
}

void UDP_readCommand(char *command)
{
    if (strncmp("help\n", command, sizeof("help")) == 0)
    {
        UDP_replyHelp();
    }
    else if (strncmp("count\n", command, sizeof("count")) == 0)
    {
        UDP_replyCount();
    }
    else if (strncmp("length\n", command, sizeof("length")) == 0)
    {
        UDP_replyLength();
    }
    else if (strncmp("history\n", command, sizeof("history")) == 0)
    {
        UDP_replyHistory();
    }
    else if (strncmp("get", command, *strtok(command, " ")) == 0)
    {

        // check if the number is integer
        int num = atoi(strtok(NULL, " "));
        UDP_replyGet(num);
    }
    else if (strncmp("dips\n", command, sizeof("dips")) == 0)
    {
        UDP_replyDips();
    }
    else if (strncmp("stop\n", command, sizeof("stop")) == 0)
    {
        UDP_replyStop();
    }
    else if (strncmp("\n", command, sizeof("")) == 0)
    {
        UDP_replyEnter();
    }

    strncpy(lastCommand, command, MAX_LEN);
    printf("Message received (%d bytes): '%s'", bytesRx, command);
}

void *UDP_threadFunc(void *arg)
{
    UDP_serverInit();
    while (readingData)
    {
        bytesRx = recvfrom(socketDescriptor, messageRx, MAX_LEN - 1, 0, (struct sockaddr *)&sinRemote, &sin_len);
        UDP_readCommand(messageRx);
    }
    return NULL;
}

void UDP_threadInit(void)
{
    pthread_create(&UDP_thread, NULL, UDP_threadFunc, NULL);
    pthread_join(UDP_thread, NULL);
}