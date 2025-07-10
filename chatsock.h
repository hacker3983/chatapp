#ifndef _CHATSOCKET_H
#define _CHATSOCKET_H
#include "chatsock_def.h"
#include "chatsock_list.h"

void chatsock_init();
chatsock_t chatsock_create(const char* addr, int port);
int chatsock_bind(chatsock_t* chatsock);
int chatsock_listen(chatsock_t* chatsock);
int chatsock_connect(chatsock_t* chatsock);
chatsock_t chatsock_accept(chatsock_t* chatsock);
int chatsock_send(chatsock_t chatsock, void* buff, size_t buffsize);
int chatsock_recv(chatsock_t chatsock, void* buff, size_t buffsize);
int chatsock_recvall(chatsock_t chatsock, void* buff, size_t buffsize);
void chatsock_close(chatsock_t* chatsock);
void chatsock_destroy();
#endif