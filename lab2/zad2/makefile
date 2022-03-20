CC=gcc
CFLAGS=-Wall

main: zad2.o
	make zad2.o
	$(CC) $(CFLAGS) -o main zad2.o

zad1.o: zad2.c
	$(CC) $(CFLAGS) -c zad2.c

clean:
	rm -f *.o main
