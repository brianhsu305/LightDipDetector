#ifndef _UDP_SOCKETS_H_
#define _UDP_SOCKETS_H_

void UDP_threadInit(void);
void *UDP_threadFunc(void);
void UDP_receiveData(void);

void UDP_replyHelp(void);
void UDP_replyCount(void);
void UDP_replyLength(void);
void UDP_replyHistory(void);
void UDP_replyGet(void);
void UDP_replyDips(void);
void UDP_replyStop(void);
void UDP_replyEnter(void);

#endif