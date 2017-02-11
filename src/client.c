#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <string.h>
#include "timer.h"
#include "common.h"
#define NUM_STR 1024
#define STR_LEN 1000

int thread_count =1000;  
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
	struct sockaddr_in sock_var;
	int clientFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	struct response res;

	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=3000;
	sock_var.sin_family=AF_INET;
	
	
	int socket =connect(clientFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var)); 
	if(socket>=0)
	{
		printf("Connected to server %dn",clientFileDescriptor);
		sendPayload(id,socket);	
		rcv_response(socket, &res);
		printf("String from Server: %s",res.msg);
		close(clientFileDescriptor);
	}else{
		printf("socket creation failed");
	}
    return NULL;	
}


int main()
{
	long thread;  /* Use long in case of a 64-bit system */
	pthread_t* thread_handles; 
	int i;
	double start, finish, elapsed;	

	/* Intializes random number generators */
	seed = malloc(thread_count*sizeof(int));
	for (i = 0; i < thread_count; i++)
		seed[i] = i;
	   
	thread_handles = malloc (thread_count*sizeof(pthread_t)); 
	pthread_mutex_init(&mutex, NULL);
	
	GET_TIME(start); 
	for (thread = 0; thread < thread_count; thread++)  
		pthread_create(&thread_handles[thread], NULL, connectToServer, (void*) thread);  

	for (thread = 0; thread < thread_count; thread++) 
		pthread_join(thread_handles[thread], NULL); 
	GET_TIME(finish);
	elapsed = finish - start;	
   
	pthread_mutex_destroy(&mutex);
	free(thread_handles);
	return 0;
}
