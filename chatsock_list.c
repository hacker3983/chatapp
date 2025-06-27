#include "chatsock.h"
#include "chatsock_list.h"

void chatsock_list_init(chatsock_list_t* sockets) {
    sockets->list = NULL;
    sockets->count = 0;
}

void chatsock_list_addsocket(chatsock_list_t* sockets, chatsock_t sock) {
    size_t new_count = sockets->count + 1;
    chatsock_t* new_sockets = realloc(sockets->list,
        new_count * sizeof(chatsock_t));
    if(!new_sockets) {
        return;
    }
    new_sockets[new_count-1] = sock;
    sockets->list = new_sockets;
    sockets->count = new_count;
}

void chatsock_list_closeall(chatsock_list_t* sockets) {
    for(size_t i=0;i<sockets->count;i++) {
        chatsock_close(&sockets->list[i]);
    }
}
void chatsock_list_destroy(chatsock_list_t* sockets) {
    chatsock_list_closeall(sockets);
    free(sockets->list);
    chatsock_list_init(sockets);
}