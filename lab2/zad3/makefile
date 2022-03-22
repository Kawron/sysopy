CC=gcc
CFLAGS=-Wall

main: zad3.o
	make zad3.o
	$(CC) $(CFLAGS) -o main zad3.o

zad3.o: zad3.c
	$(CC) $(CFLAGS) -c zad3.c

clean:
	rm -f *.o main
