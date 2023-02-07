#ifndef _UDP_SOCKETS_H_
#define _UDP_SOCKETS_H_

// Initialize the UDP thread
void UDP_threadInit(void);
void *UDP_threadFunc(void *arg);

// read the user input command
void UDP_readCommand(char *command);

// client response for each command
void UDP_replyHelp(void);
void UDP_replyCount(void);
void UDP_replyLength(void);
void UDP_replyHistory(void);
void UDP_replyGet(int num);
void UDP_replyDips(void);
void UDP_replyStop(void);
void UDP_replyEnter(void);

#endif