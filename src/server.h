#ifndef __SERVER_H__
#define __SERVER_H__

// Server Tunables
#define THREAD_COUNT 20
#define ARRAY_PARTITIONS 1


// Globals
char** array;

// Functions
int read_lock(index);
int read_unlock(index);

int write_lock(index);
int write_unlock(index);

int read_index(uint32_t index, char* buff);
int write_index(uint32_t index, char* buff);

void* handle_request(void* args);

void parse_args(int argc, char** argv, int* port, uint32_t* n);
int init_socket(int port);
void init_array(uint32_t n);
void init_protection();

#endif
