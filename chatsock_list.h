#ifndef _CHATSOCK_LIST_H
#define _CHATSOCK_LIST_H
#include "chatsock_def.h"

void chatsock_list_init(chatsock_list_t* sockets);
void chatsock_list_addsocket(chatsock_list_t* sockets, chatsock_t sock);
void* chatsock_list_accept(void* arg);
pthread_t chatsock_list_start_accept(chatsock_acceptarg_t* acception_arg);
void chatsock_list_join_accept(pthread_t acception_thread);
void* chatsock_list_recv_message(void* arg);
void chatsock_list_start_recv(chatsock_list_t* sockets, size_t index);
char* chatsock_list_join_recv(chatsock_list_t* sockets, size_t index);
void chatsock_list_closeall(chatsock_list_t* sockets);
void chatsock_list_destroy(chatsock_list_t* sockets);
#endif