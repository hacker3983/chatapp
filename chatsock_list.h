#ifndef _CHATSOCK_LIST_H
#define _CHATSOCK_LIST_H
#include "chatsock_def.h"

void chatsock_list_init(chatsock_list_t* sockets);
void chatsock_list_addsocket(chatsock_list_t* sockets, chatsock_t sock);
void chatsock_list_closeall(chatsock_list_t* sockets);
void chatsock_list_destroy(chatsock_list_t* sockets);
#endif