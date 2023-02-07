#ifndef _UDP_SOCKETS_H_
#define _UDP_SOCKETS_H_

void UDP_threadInit(void);
void *UDP_threadFunc(void);
void UDP_receiveData(void);
void UDP_reply(void);

#endif