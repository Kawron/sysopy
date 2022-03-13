app: zad1.o libzad1.a zad2.o
	gcc -o app zad2.o -L. -lzad1

libzad1.a: zad1.o
	ar cr libzad1.a zad1.o

zad2.o: zad2.c
	gcc $(CFLAGS) -c zad2.c

zad1.o: zad1.c
	gcc $(CFLAGS) -c zad1.c

clean:
	rm -f *.o app libzad1.a
