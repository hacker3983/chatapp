#include <stdio.h>
#include "chatserver.h"

void* broadcast_message(void* arg) {
    broadcast_args_t* broadcast_args = (broadcast_args_t*)arg;
    chatsock_list_t* client_list = broadcast_args->client_list;
    for(size_t i=0;i<client_list->count;i++) {
        if(client_list->list[i].fd == broadcast_args->client.fd) {
            continue;
        }
        chatsock_send(client_list->list[i], broadcast_args->msg, strlen(broadcast_args->msg));
    }
}

void* recv_message(void* arg) {
    communication_args_t* receive_args = (communication_args_t*)arg,
        broadcast_args = *receive_args;
    chatsock_t client_sock = receive_args->client;
    while(1) {
        char buff[1024] = {0};
        chatsock_recv(client_sock, buff, sizeof(buff));
        broadcast_args.msg = buff;
        pthread_t broadcast_thread;
        printf("Received message from client: %s\n", buff);
        printf("Creating and broacasting message to clients\n");
        pthread_create(&broadcast_thread, NULL, broadcast_message, &broadcast_args);
        pthread_join(broadcast_thread, NULL);
    }
}

int main() {
    chatsock_init();
    chatsock_t sock = chatsock_create("127.0.0.1", PORT);
    if(chatsock_bind(&sock) < 0) {
        fprintf(stderr, "Failed to bind to the socket\n");
    }
    if(chatsock_listen(&sock) < 0) {
        fprintf(stderr, "Failed to listen on the socket\n");
    }
    printf("Listening on 0.0.0.0:%d\n", PORT);
    chatsock_list_t client_list = {0};
    communication_args_t receive_args = {0};
    while(1) {
        chatsock_t csock = {0};
        csock = chatsock_accept(&sock);
        printf("Got connection from %s:%d\n", inet_ntoa(csock.addr.sin_addr),
            ntohs(csock.addr.sin_port));
        pthread_t recv_thread;
        chatsock_list_addsocket(&client_list, csock);
        receive_args.client = csock;
        receive_args.client_list = &client_list;
        pthread_create(&recv_thread, NULL, &recv_message, &receive_args);
    }
    chatsock_list_destroy(&client_list);
    chatsock_close(&sock);
    chatsock_destroy();
    return 0;
}