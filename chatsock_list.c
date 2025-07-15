#include <stdio.h>
#include "chatsock.h"
#include "chatsock_list.h"

void chatsock_list_init(chatsock_list_t* sockets) {
    sockets->list = NULL;
    sockets->count = 0;
}

void chatsock_list_mutex_init(chatsock_list_t* sockets) {
    pthread_mutex_init(&sockets->mutex, NULL);
}

void chatsock_list_mutex_lock(chatsock_list_t* sockets) {
    pthread_mutex_lock(&sockets->mutex);
}

void chatsock_list_mutex_unlock(chatsock_list_t* sockets) {
    pthread_mutex_unlock(&sockets->mutex);
}

void chatsock_list_addsocket(chatsock_list_t* sockets, chatsock_t sock, size_t uid) {
    size_t new_count = sockets->count + 1;
    chatsock_item_t* new_sockets = realloc(sockets->list,
        new_count * sizeof(chatsock_item_t));
    if(!new_sockets) {
        return;
    }
    memset(new_sockets + sockets->count, 0, sizeof(chatsock_item_t));
    new_sockets[new_count-1].sock = sock;
    new_sockets[new_count-1].uid = uid;
    sockets->list = new_sockets;
    sockets->count = new_count;
}

bool chatsock_list_find(chatsock_list_t* sockets, size_t uid, size_t* index) {
    bool found = false;
    for(size_t i=0;i<sockets->count;i++) {
        if(sockets->list[i].uid == uid) {
            *index = i;
            found = true;
            break;
        }
    }
    return found;
}

bool chatsock_list_remove(chatsock_list_t* sockets, size_t index) {
    if(index >= sockets->count) {
        return false;
    }
    for(size_t i=index;i<sockets->count-1;i++) {
        chatsock_item_t current = sockets->list[i];
        sockets->list[i] = sockets->list[i+1];
        sockets->list[i+1] = current;
    }
    sockets->count--;
    if(!sockets->count) {
        free(sockets->list);
        chatsock_list_init(sockets);
        return true;
    }
    sockets->list = realloc(sockets->list, sockets->count
        * sizeof(chatsock_item_t));
    return true;
}

void chatsock_list_setdisconnected_status(chatsock_list_t* sockets, size_t uid, bool status) {
    size_t client_index = 0;
    if(chatsock_list_find(sockets, uid, &client_index)) {
        sockets->list[client_index].disconnected = status;
    }
}

void chatsock_list_setrecv_status(chatsock_list_t* sockets, size_t uid, bool status) {
    size_t client_index = 0;
    if(chatsock_list_find(sockets, uid, &client_index)) {
        sockets->list[client_index].recv_finished = status;
    }
}

void* chatsock_list_accept(void* arg) {
    chatsock_acceptarg_t* acception_arg = (chatsock_acceptarg_t*)arg;
    chatsock_t sock = acception_arg->sock;
    chatsock_list_t* client_list = acception_arg->client_list;
    size_t uid = 0;
    while(1) {
        chatsock_t csock = {0};
        csock = chatsock_accept(&sock);
        printf("Got connection from %s:%d\n", inet_ntoa(csock.addr.sin_addr),
            ntohs(csock.addr.sin_port));
        chatsock_list_mutex_lock(client_list);
        chatsock_list_addsocket(client_list, csock, uid);
        chatsock_list_start_recv(client_list, client_list->count-1);
        chatsock_list_mutex_unlock(client_list);
        uid++;
    }
}

pthread_t chatsock_list_start_accept(chatsock_acceptarg_t* acception_arg) {
    pthread_t new_thread;
    pthread_create(&new_thread, NULL, &chatsock_list_accept, acception_arg);    
    return new_thread;
}
void chatsock_list_join_accept(pthread_t acception_thread) {
    pthread_join(acception_thread, NULL);
}

void* chatsock_list_recv_message(void* arg) {
    chatsock_recvarg_t* recv_arg = (chatsock_recvarg_t*)arg;
    chatsock_list_t* client_list = recv_arg->client_list;
    size_t client_index = recv_arg->client_index,
        message_size = 0, uid = client_list->list[client_index].uid;
    chatsock_t client = client_list->list[client_index].sock;
    if(chatsock_recv(client, &message_size, sizeof(size_t)) <= 0) {
        chatsock_close(&client);
        chatsock_list_mutex_lock(client_list);
        chatsock_list_setdisconnected_status(client_list, uid, true);
        chatsock_list_mutex_unlock(client_list);
        printf("Client %zu Disconnected from server!\n", client_index);
        free(recv_arg);
        return NULL;
    }
    char* message = calloc(message_size+1, sizeof(char));
    if(chatsock_recvall(client, message, message_size) <= 0) {
        chatsock_close(&client);
        chatsock_list_mutex_lock(client_list);
        chatsock_list_setdisconnected_status(client_list, uid, true);
        chatsock_list_mutex_unlock(client_list);
        printf("Client %zu Disconnected from server!\n", client_index);
        free(recv_arg);
        return NULL;
    }
    printf("Received message buffer: %s\n", message);
    chatsock_list_mutex_lock(client_list);
    chatsock_list_setrecv_status(client_list, uid, true);
    chatsock_list_mutex_unlock(client_list);
    free(recv_arg);
    return message;
}

void chatsock_list_start_recv(chatsock_list_t* sockets, size_t index) {
    pthread_t new_thread;
    chatsock_recvarg_t* recv_arg = malloc(sizeof(chatsock_recvarg_t));
    recv_arg->client_index = index;
    recv_arg->client_list = sockets;
    pthread_create(&new_thread, NULL, &chatsock_list_recv_message, recv_arg);
    sockets->list[index].recv_thread = new_thread;
    sockets->list[index].recv_started = true;
}

char* chatsock_list_join_recv(chatsock_list_t* sockets, size_t index) {
    char* message = NULL;
    pthread_join(sockets->list[index].recv_thread, (void**)&message);
    sockets->list[index].recv_started = false;
    sockets->list[index].recv_finished = false;
    return message;
}

void chatsock_list_closeall(chatsock_list_t* sockets) {
    for(size_t i=0;i<sockets->count;i++) {
        chatsock_close(&sockets->list[i].sock);
    }
}
void chatsock_list_destroy(chatsock_list_t* sockets) {
    pthread_mutex_destroy(&sockets->mutex);
    chatsock_list_closeall(sockets);
    free(sockets->list);
    chatsock_list_init(sockets);
}