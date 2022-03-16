all: shared_lib static_lib dynamic_lib
	make shared_lib
	make static_lib
	make dynamic_lib

shared_lib: zad1.c
	gcc $(CFLAGS) -c -fPIC zad1.c
	gcc $(CFLAGS) -shared -fPIC -o libzad1shared.so zad1.o

static_lib: zad1.c
	gcc $(CFLAGS) -c zad1.c
	ar cr libzad1static.a zad1.o

clean:
	rm *.o app libzad1static.a libzad1shared.so
