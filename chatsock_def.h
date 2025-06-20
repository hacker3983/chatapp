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

typedef struct chatsock {
    chatsock_fd_t fd;
    struct sockaddr_in addr;
    #ifdef _WIN32
    int addr_len;
    #else
    socklen_t addr_len;
    #endif
} chatsock_t;
#endif