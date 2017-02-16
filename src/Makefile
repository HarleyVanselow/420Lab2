all: client server
client: client.c
	gcc -std=gnu99 -Wall -Werror -pthread -o client common.c client.c
server: server.c
	gcc -std=gnu99 -Wall -Werror -pthread -o server common.c server.c
clean:
	rm client server

