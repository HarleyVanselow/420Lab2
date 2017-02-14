all: client server
client:
	gcc -std=gnu99 -pthread -o client common.c client.c
server:
	gcc -std=gnu99 -pthread -o server common.c server.c
clean:
	rm client server

