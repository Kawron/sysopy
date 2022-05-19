CC=gcc
CFLAGS= -Wall

all: main.c cook.c currier.c
	$(CC) $(CFLAGS) main.c -o main -lm
	$(CC) $(CFLAGS) cook.c -o cook -lm
	$(CC) $(CFLAGS) currier.c -o currier -lm

clean:
	rm -f *.o main cook currier
