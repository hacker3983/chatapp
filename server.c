#include <stdio.h>
#include "chatsock.h"
#define PORT 7777

int main() {
    chatsock_init();
    chatsock_t sock = chatsock_create("127.0.0.1", PORT);
    if(chatsock_bind(&sock) < 0) {
        fprintf(stderr, "Failed to bind to the socket\n");
    }
    if(chatsock_listen(&sock) < 0) {
        fprintf(stderr, "Failed to listen on the socket\n");
    }
    printf("LIstening on 0.0.0.0:%d\n", PORT);
    chatsock_t csock = {0};
    while(1) {
       csock = chatsock_accept(&sock);
       printf("Got connection from %s:%d\n", inet_ntoa(csock.addr.sin_addr),
            ntohs(csock.addr.sin_port));
        char msg[] = "Hello, Welcome to the server! How are you today?",
            buff[1024] = {0};
        chatsock_send(csock, msg, sizeof(msg));
        chatsock_recv(csock, buff, 1024);
        printf("Received message from client: %s\n", buff);
        chatsock_close(&csock);
    }
    chatsock_close(&sock);
    chatsock_destroy();
    return 0;
}