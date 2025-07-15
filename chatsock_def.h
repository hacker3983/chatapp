#ifndef _CHATSOCK_DEF_H
#define _CHATSOCK_DEF_H
#ifdef _WIN32
#ifndef _WIN32_LEAN_AND_MEAN
#define _WIN32_LEAN_AND_MEAN
#endif
#pragma comment("ws2_32.lib")
#include <winsock2.h>
#include <windows.h>
typedef SOCKET chatsock_fd_t;
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int chatsock_fd_t;
#endif
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef struct chatsock {
    chatsock_fd_t fd;
    struct sockaddr_in addr;
    #ifdef _WIN32
    int addr_len;
    #else
    socklen_t addr_len;
    #endif
} chatsock_t;

typedef struct chatsock_item {
    size_t uid;
    chatsock_t sock;
    pthread_t recv_thread;
    bool recv_started;
    bool recv_finished;
    bool disconnected;
} chatsock_item_t;

typedef struct chatsock_list {
    pthread_mutex_t mutex;
    chatsock_item_t* list;
    size_t count;
} chatsock_list_t;

typedef struct chatsock_acceptarg {
    chatsock_t sock;
    chatsock_list_t* client_list;
} chatsock_acceptarg_t;

typedef struct chatsock_recvarg {
    chatsock_list_t* client_list;
    size_t client_index;
} chatsock_recvarg_t;
#endif
