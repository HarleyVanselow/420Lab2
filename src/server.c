#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>

#include<stdint.h>
#include"common.h"
#include"server.h"


int main(int argc, char** argv){
    pthread_t t[THREAD_COUNT];
    int ssock, csock, port, n, result;

    parse_args(argc, argv, &port, &n);
    ssock = init_socket(port);
    init_array(n);

    while(1){
        for(int i=0;i<THREAD_COUNT;i++){
            csock=accept(ssock,NULL,NULL);
            pthread_create(
                    &t[i],
                    NULL,
                    handle_request,
                    (void *) csock
            );
        }
        for(int i=0;i<THREAD_COUNT; i++){
            pthread_join(&t[i], (void *) &result);
        }
    }
    close(ssock);

return 0;
}

void *handle_request(void *args){
    struct request req;
    struct response res;

    int sock = (int) args;

    if(rcv_request(sock, &req) == - 1){
            strncpy(res.msg,"Unable to receive request",MSG_SIZE);
        if (req.type == REQ_RD){
            if(read_index(req.index, res.msg) == -1){
                sprintf(res.msg, "Unable to read index %lu", req.index);
            }
        } else if(req.type == REQ_WR) {
            if(write_index(req.index, res.msg) == -1){
                sprintf(res.msg, "Unable to write index %lu", req.index);
            }
        } else {
            strncpy(res.msg,"Invalid type",MSG_SIZE);
        }

        snd_response(sock, &res);
        close(sock);
    }
}

int init_socket(int port){
    struct sockaddr_in sock_var;
    int ssock=socket(AF_INET,SOCK_STREAM,0);

    sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
    sock_var.sin_port=3000;
    sock_var.sin_family=AF_INET;

    if(bind(ssock,
            (struct sockaddr*)&sock_var,
            sizeof(sock_var)
        ) >=0 ){
		listen(ssock,2000); 
    } else {
        printf("Unable to bind to socket\n");
        exit(EXIT_FAILURE);
        
    }
}

void init_array(uint32_t n){
    char* tmp;

    array = malloc(sizeof(char*) * n);
    if (array == NULL){
        printf("Unable to allocate array");
        exit(EXIT_FAILURE);
    }

    for(uint32_t i=0; i < n; i++){
        tmp = malloc(sizeof(char) * MSG_SIZE);
        if (tmp == NULL){
            printf("Unable to allocate string %lu\n", i);
            exit(EXIT_FAILURE);
        }    

        sscanf(tmp, "String %lu: the initial value.", i);
    }
};

int write_index(uint32_t index, char* buff){
    int s;
    write_lock(index);
    sscanf(array[index], 
            "String %d has been modified by a write request",
            index);
    strncpy(buff, array[index], MSG_SIZE);
    write_unlock(index);
}

int read_index(uint32_t index, char* buff){
    int s;
    read_lock(index);
    strncpy(buff, array[index], MSG_SIZE);
    read_unlock(index);
}
