CC = gcc -Wall -ggdb

all: server client

client: client.c
	$(CC) client.c -o ./client

server: server.c
	$(CC) server.c -o ./server

clean:
	rm -rf *.o *.txt server client
