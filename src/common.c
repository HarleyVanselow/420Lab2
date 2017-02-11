#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <netinet/in.h>

#include "common.h"


void snd_request(int socket, struct request* req){
    uint32_t nindex = htonl(req->index);
    uint32_t ntype = htonl(req->type);

    write(socket, &nindex, sizeof(uint32_t));
    write(socket, &ntype, sizeof(uint32_t));

}

int rcv_request(int socket, struct request* req){
    uint32_t nindex, ntype;

    if (read(socket, &nindex, sizeof(uint32_t)) == -1){
            return -1;
    }

    if(read(socket, &ntype, sizeof(uint32_t)) == -1){
        return -1;
    }

    req->index = ntohl(nindex);
    req->type = ntohl(ntype);

    return 0;
}

void snd_response(int socket, struct response* res){
    write(socket, res->msg, MSG_SIZE);
}

void rcv_response(int socket, struct response* res){
    read(socket, res->msg, MSG_SIZE);
}
