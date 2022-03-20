CC=gcc
CFLAGS=-Wall

main: zad1.o
	make zad1.o
	$(CC) $(CFLAGS) -o main zad1.o

zad1.o: zad1.c
	$(CC) $(CFLAGS) -c zad1.c

clean:
	rm -f *.o main
