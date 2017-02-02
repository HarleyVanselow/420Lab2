#ifndef __COMMON_H__
#define __COMMON_H__

#define MSG_SIZE 32
#define REQ_RD 0
#define REQ_WR 1

struct request{
    uint32_t index;
    uint32_t type;
};

struct response{
    char[MSG_SIZE] msg;
}

void snd_request(int socket, request* req);
void rcv_request(int socket, request* req);
void snd_response(int socket, response* res);
void rcv_response(int socket, response* res);

#endif
