#include <stdio.h>
#include "chatsock.h"
#define PORT 7777

int main() {
    chatsock_init();
    chatsock_t sock = chatsock_create("127.0.0.1", PORT);
    char msg[] = "Hello, There Server I am doing great!",
         buff[1024] = {0};
    chatsock_connect(&sock);
    chatsock_send(sock, msg, sizeof(msg));
    chatsock_recv(sock, buff, 1024);
    printf("Received message from server: %s\n", buff);
    chatsock_close(&sock);
    chatsock_destroy();
    return 0;
}