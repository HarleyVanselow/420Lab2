#include <stdio.h>
#include <stdint.h>
#include <netinet/in.h>

#include "common.h"


snd_request(int socket, request* req){
    uint32_t nindex = htonl(req->index);
    uint32_t ntype = htonl(req->type);

    write(socket, &nindex, sizeof(uint32_t));
    write(socket, &ntype, sizeof(uint32_t));

}

rcv_request(int socket, request* req){
    uint32_t nindex, ntype;

    if (read(socket, &nindex, sizeof(uint32_t)) == -1){
            return -1;
    }

    if(read(socket, &ntype, sizeof(uint32_t)) == -1){
        return -1;
    }

    req->index = ntohl(nindex);
    req->type = ntohl(ntype);
}

snd_response(int socket, response* res){
    write(int socket, response->msg, MSG_SIZE);
}

rcv_response(int socket, response* res){
    read(int socket, response->msg, MSG_SIZE);
}
