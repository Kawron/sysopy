CC=gcc
CFLAGS=-Wall

all:
	make static_lib
	make shared_lib

shared_lib: zad1.c
	$(CC) $(CFLAGS) -c -fPIC zad1.c
	$(CC) -shared -fPIC -o libzad1shared.so zad1.o

static_lib: zad1.c
	$(CC) $(CFLAGS) -c zad1.c
	ar cr libzad1static.a zad1.o

clean:
	rm -f *.o app libzad1static.a libzad1shared.so
