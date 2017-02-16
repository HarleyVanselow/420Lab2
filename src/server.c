#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>
#include"timer.h"
#include<stdint.h>
#include"common.h"
#include"server.h"


int array_divisions;
pthread_rwlock_t* lock_array;


int main(int argc, char** argv){
    pthread_t* t;
    int ssock, port, n, thread_count;
	double result,total_duration;
    intptr_t csock;
	

    parse_args(argc, argv, &port, &n, &thread_count, &array_divisions);
    ssock = init_socket(port);
    t = allocate_threads(thread_count);
    init_protection(array_divisions);
    init_array(n);
	int connections_processed_count=0;
    while(1){
        for(int i=0;i<thread_count;i++){
            csock=accept(ssock,NULL,NULL);
            pthread_create(
                    &t[i],
                    NULL,
                    handle_request,
                    (void *) csock
            );
        }
        for(int i=0;i<thread_count; i++){
            pthread_join(t[i], (void *) &result);
			total_duration+=result;
			printf("Total duration after thread %d: %lf\n",i,total_duration);
			connections_processed_count++;
        }
		if(connections_processed_count >= CLIENT_THREADS){
			break;
		}
    }
    close(ssock);
return total_duration;
}

void* handle_request(void *args){
    struct request req;
    struct response res;

    int sock = (intptr_t) args;
	double* access_duration = malloc(sizeof(double));
    if(rcv_request(sock, &req) == - 1){
            strncpy(res.msg,"Unable to receive request",MSG_SIZE);
	}else{
		if (req.type == REQ_RD){
			*access_duration = read_index(req.index, res.msg);
			if( *access_duration == -1){
				sprintf(res.msg, "Unable to read index %u", req.index);
			}
		} else if(req.type == REQ_WR) {
			*access_duration = write_index(req.index, res.msg) ;
			if(*access_duration == -1){
				sprintf(res.msg, "Unable to write index %u", req.index);
			}
		} else {
			printf("Recieved invalid type\n");
			strncpy(res.msg,"Invalid type",MSG_SIZE);
		}	
	}
	
	snd_response(sock, &res);
	close(sock);
    

    return access_duration;
}

int init_socket(int port){
    struct sockaddr_in sock_var;
    int ssock=socket(AF_INET,SOCK_STREAM,0);

    sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
    sock_var.sin_port=port;
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

    return ssock;
}

pthread_t*  allocate_threads(int thread_count){
   pthread_t* t = (pthread_t*) malloc(thread_count * sizeof(pthread_t));
   if(t == NULL){
        printf("Unable to allocate threads\n");
        exit(EXIT_FAILURE);
   }
   return t;
}

void init_array(int n){
    char* tmp;
    array = malloc(sizeof(char*) * n);
    if (array == NULL){
        printf("Unable to allocate array");
        exit(EXIT_FAILURE);
    }

    for(uint32_t i=0; i < n; i++){
        tmp = malloc(sizeof(char) * MSG_SIZE);
        if (tmp == NULL){
            printf("Unable to allocate string %u\n", i);
            exit(EXIT_FAILURE);
        }    

        sprintf(tmp, "String %u: the initial value.", i);
		array[i] = tmp;
    }
};

double write_index(uint32_t index, char* buff){
	double start,end;
	GET_TIME(start);
    write_lock(index);
    sprintf(array[index], 
            "String %u has been modified by a write request",
            index);
    strncpy(buff, array[index], MSG_SIZE);
    write_unlock(index);
	GET_TIME(end);
    return end-start;
}

double read_index(uint32_t index, char* buff){
	int start,end;
	GET_TIME(start); 
    read_lock(index);
    strncpy(buff, array[index], MSG_SIZE);
    read_unlock(index);
	GET_TIME(end);
    return end-start;
}

void read_unlock(uint32_t index){
    pthread_rwlock_unlock(&lock_array[index%array_divisions]);
}

void read_lock(uint32_t index){
    pthread_rwlock_rdlock(&lock_array[index%array_divisions]);
}

void write_unlock(uint32_t index){
    pthread_rwlock_unlock(&lock_array[index%array_divisions]);
}

void write_lock(uint32_t index){
    pthread_rwlock_wrlock(&lock_array[index%array_divisions]);
}

void init_protection(int array_divisions){
    lock_array = malloc(array_divisions * sizeof(pthread_rwlock_t));   
    if(lock_array == NULL){
            printf("Unable to allocate lock array\n");
            exit(EXIT_FAILURE);
    }
    for(int i = 0; i < array_divisions; i++){
        if(pthread_rwlock_init(&lock_array[i], NULL) != 0){
            printf("Unable to initialize lock %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
}

void parse_args(int argc, char** argv, 
        int* port, int* n,
        int* thread_count, int* array_divisions){

        if(argc < 3 || argc > 5){
            printf("Usage: %s <port> <n> [<thread_count> <array_divisions>]\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        *port = atoi(argv[1]);
        if(*port == 0){
            printf("Please enter a valid port number\n");
            exit(EXIT_FAILURE);
        }
        *n = atoi(argv[2]);
        if(*n <= 0){
            printf("Please enter a positive number for the array size");
            exit(EXIT_FAILURE);
        }

        *thread_count = 20;
        *array_divisions = 1;

        if(argc > 3){
            *thread_count = atoi(argv[3]);
            if(*n <= 0){
                printf("Please enter a positive number for the thread count");
                exit(EXIT_FAILURE);
            }

            *array_divisions = atoi(argv[4]);
            if(*n <= 0){
                printf("Please enter a positive number for the array_divisions");
                exit(EXIT_FAILURE);
            }

        }
}


