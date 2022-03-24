CC=gcc
CFLAGS=-Wall

main: zad1.o
	make zad1.o
	$(CC) $(CFLAGS) -o main zad1.o

zad1.o: zad1.c
	$(CC) $(CFLAGS) -c zad1.c

clean:
	rm -f *.o main raport.txt

test:
	make clean
	make main
	@printf "Time Test: \n" >> raport.txt
	@./main from.txt to.txt >> raport.txt
