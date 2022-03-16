app:
	make zad2.o
	make -C ../zad1 static_lib
	gcc $(CFLAGS) -o app zad2.o -I../zad1 -L../zad1 -lzad1static

zad2.o: zad2.c
	gcc $(CFLAGS) -c zad2.c

test:
	clean
	@printf "Test 1: \n" >> raport2.txt
	@./app `cat args1.txt` >> raport2.txt
	@printf "Test 2: \n" >> raport2.txt
	@./app `cat args2.txt` >> raport2.txt
	@printf "Test 3: \n" >> raport2.txt
	@./app `cat args3.txt` >> raport2.txt
	@printf "Test 4: \n" >> raport2.txt
	@./app `cat args4.txt` >> raport2.txt
	@cat raport2.txt


clean:
	rm *.o app libzad1.a raport2.txt
