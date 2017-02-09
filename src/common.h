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
    char msg[MSG_SIZE];
};

void snd_request(int socket, struct request* req);
int rcv_request(int socket, struct request* req);
void snd_response(int socket, struct response* res);
void rcv_response(int socket, struct response* res);

#endif
