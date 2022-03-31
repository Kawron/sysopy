CC=gcc
CFLAGS=-Wall

main:
	$(CC) $(CFLAGS) -o main zad2.c
	$(CC) $(CFLAGS) -o calc calculate.c -lm

clean:
	rm -f *.o main *.txt calc

raport:
	make clean
	make main
	@printf "Podział 100: \n" >> raport1
	@./main 100 >> raport1
	@printf "Podział 1000: \n" >> raport1
	@./main 1000 >> raport1
	@printf "Podział 10000: \n" >> raport1
	@./main 10000 >> raport1
	make clean
	mv raport1 raport1.txt
