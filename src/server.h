#ifndef __SERVER_H__
#define __SERVER_H__

// Globals
char** array;

// Functions
void read_lock(uint32_t index);
void read_unlock(uint32_t index);
void write_lock(uint32_t index);
void write_unlock(uint32_t index);

int read_index(uint32_t index, char* buff);
int write_index(uint32_t index, char* buff);

void* handle_request(void* args);

void parse_args(int argc, char** argv, 
        int* port, int* n, 
        int* thread_count, int* array_divisions);
int init_socket(int port);
void init_array(int n);
void init_protection(int array_divisions);
pthread_t* allocate_threads(int thread_count);

#endif
