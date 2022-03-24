CC=gcc
CFLAGS=-Wall

main: zad2.o
	make clean
	make zad2.o
	$(CC) $(CFLAGS) -o main zad2.o

zad1.o: zad2.c
	$(CC) $(CFLAGS) -c zad2.c

clean:
	rm -f *.o main raport.txt

test:
	make clean
	make main
	@printf "Time Test: \n" >> raport.txt
	@./main k test.txt >> raport.txt
