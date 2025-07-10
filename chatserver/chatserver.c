#include <stdio.h>
#include "chatserver.h"

void broadcast_message(chatsock_list_t* client_list, size_t index, char* message) {
    chatsock_t client = client_list->list[index].sock;
    size_t message_size = strlen(message);
    for(size_t i=0;i<client_list->count;i++) {
        if(client_list->list[i].sock.fd == client.fd) {
            continue;
        }
        chatsock_send(client_list->list[i].sock, &message_size, sizeof(size_t));
        chatsock_send(client_list->list[i].sock, message, message_size);
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
    chatsock_acceptarg_t acception_arg = {
        .client_list = &client_list,
        .sock = sock
    };
    pthread_t acception_thread = chatsock_list_start_accept(&acception_arg);
    while(1) {
        for(size_t i=0;i<client_list.count;i++) {
            if(client_list.list[i].recv_started
                && client_list.list[i].recv_finished) {
                char* message = chatsock_list_join_recv(&client_list, i);
                printf("Message received from client %zu: %s\n", i, message);
                printf("Broadcasting / relaying message to other clients!\n");
                broadcast_message(&client_list, i, message);
                free(message);
                chatsock_list_start_recv(&client_list, i);
            }
        }
    }
    chatsock_list_join_accept(acception_thread);
    chatsock_list_destroy(&client_list);
    chatsock_close(&sock);
    chatsock_destroy();
    return 0;
}