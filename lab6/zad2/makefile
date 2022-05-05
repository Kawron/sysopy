CC = gcc -Wall -ggdb

all: server client

client: client.c
	$(CC) client.c -o ./client -lrt

server: server.c
	$(CC) server.c -o ./server -lrt

clean:
	rm -rf *.o *.txt server client
