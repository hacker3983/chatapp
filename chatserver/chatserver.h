#ifndef _CHATSERVER_H
#define _CHATSERVER_H
#include "../chatsock_def.h"
#include "../chatsock.h"
#include <pthread.h>
#define PORT 7777

typedef struct communication_args {
    chatsock_t client;
    chatsock_list_t* client_list;
    char* msg;
} communication_args_t, broadcast_args_t, recv_args_t;
#endif