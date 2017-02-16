#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netinet/tcp.h>
#include<unistd.h>
#include<pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <string.h>
#include<errno.h>
#include "timer.h"
#include "common.h"
#define STR_LEN 1000

int NUM_STR;

int port;
unsigned int* seed;
pthread_mutex_t mutex;


void sendPayload(void* rank, int socket) {
	long my_rank = (long) rank;
	
	// Find a random position in theArray for read or write
	int pos = rand_r(&seed[my_rank]) % NUM_STR;
	int randNum = rand_r(&seed[my_rank]) % 100;	// write with 10% probability
	struct request req;
	req.index = pos;
	if (randNum >= 95) // 10% are write operations, others are reads
	{
		req.type = REQ_WR;
	
		snd_request(socket,&req);
	}else{
//issue read command
		req.type = REQ_RD;
		snd_request(socket,&req);
	}
}
void* connectToServer(void* id)
{
//	printf("Created thread %d",(int)id);
	struct sockaddr_in sock_var;
	int clientFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	struct response res;

	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=port;
	sock_var.sin_family=AF_INET;
	
	int socket =connect(clientFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var)); 
	if(socket>=0)
	{	
		sendPayload(id,clientFileDescriptor);
		
		rcv_response(clientFileDescriptor, &res);
		printf("Thread %ld: Recieved from server: %s\n",(long)id,res.msg);
		close(clientFileDescriptor);
	}else{
		printf("socket creation failed\n");
	}
    return NULL;	
}


int main(int argc, char** argv)
{
	port = atoi(argv[1]);
	NUM_STR = atoi(argv[2]);
	long thread;  /* Use long in case of a 64-bit system */
	pthread_t* thread_handles; 
	int i;
		

	/* Intializes random number generators */
	seed = malloc(CLIENT_THREADS*sizeof(int));
	for (i = 0; i < CLIENT_THREADS; i++)
		seed[i] = i;
	   
	thread_handles = malloc (CLIENT_THREADS*sizeof(pthread_t)); 
	pthread_mutex_init(&mutex, NULL);
	
	
	for (thread = 0; thread < CLIENT_THREADS; thread++)  
		pthread_create(&thread_handles[thread], NULL, connectToServer, (void*) thread);  

	for (thread = 0; thread < CLIENT_THREADS; thread++) 
		pthread_join(thread_handles[thread], NULL); 
	
	
   
	pthread_mutex_destroy(&mutex);
	free(thread_handles);
	return 0;
}
