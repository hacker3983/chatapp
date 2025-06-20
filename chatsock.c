#include "chatsock.h"

void chatsock_init() {
    #ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    #else
    #endif
}

chatsock_t chatsock_create(const char* addr, int port) {
    chatsock_t new_sock = {0};
    struct sockaddr_in new_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(addr),
        .sin_port = htons(port)
    };
    new_sock.addr = new_addr;
    new_sock.fd = socket(AF_INET, SOCK_STREAM, 0);
    return new_sock;
}

int chatsock_bind(chatsock_t* chatsock) {
    return bind(chatsock->fd,
        (struct sockaddr*)&chatsock->addr,
        sizeof(chatsock->addr));
}
int chatsock_listen(chatsock_t* chatsock) {
    return listen(chatsock->fd, 0);
}

int chatsock_connect(chatsock_t* chatsock) {
    return connect(chatsock->fd,
        (struct sockaddr*)&chatsock->addr,
        sizeof(chatsock->addr));
}

chatsock_t chatsock_accept(chatsock_t* chatsock) {
    chatsock_t new_client = {0};
    new_client.addr_len = sizeof(new_client.addr);
    new_client.fd = accept(chatsock->fd, (struct sockaddr*)&new_client.addr, &new_client.addr_len);
    return new_client;
}

int chatsock_send(chatsock_t chatsock, void* buff, size_t buffsize) {
    return send(chatsock.fd, buff, buffsize, 0);
}

int chatsock_recv(chatsock_t chatsock, void* buff, size_t buffsize) {
    return recv(chatsock.fd, buff, buffsize, 0);
}

void chatsock_close(chatsock_t* chatsock) {
    #ifdef _WIN32
    closesocket(chatsock->fd);
    #else
    close(chatsock->fd);
    #endif
}

void chatsock_destroy() {
    #ifdef _WIN32
    WSACleanup();
    #endif
}