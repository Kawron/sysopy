CC=gcc
CFLAGS=-Wall

app:
	make zad2.o
	make -C ../zad1 static_lib
	$(CC) $(CFLAGS) -o app zad2.o -I../zad1 -L../zad1 -lzad1static

zad2.o: zad2.c
	$(CC) $(CFLAGS) -c zad2.c

test:
	make clean
	make app
	@printf "Test 1: \n" >> raport2.txt
	@./app `cat ../args/args1.txt` >> raport2.txt
	@printf "Test 2: \n" >> raport2.txt
	@./app `cat ../args/args2.txt` >> raport2.txt
	@printf "Test 3: \n" >> raport2.txt
	@./app `cat ../args/args3.txt` >> raport2.txt
	@printf "Test 4: \n" >> raport2.txt
	@./app `cat ../args/args4.txt` >> raport2.txt
	@cat raport2.txt


clean:
	rm -f *.o app libzad1.a raport2.txt
