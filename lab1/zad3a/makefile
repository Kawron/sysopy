CC=gcc
CFLAGS=-Wall

static:
	# upewniam się że jest lib
	make -C ../zad1 static_lib
	$(CC) $(CFLAGS) -o appStatic ../zad2/zad2.c -L../zad1 -lzad1static

shared:
	make -C ../zad1 shared_lib
	$(CC) $(CFLAGS) -o appShared ../zad2/zad2.c -L../zad1 -lzad1shared -Wl,-rpath,../zad1

dynamic:
	make -C ../zad1 shared_lib
	$(CC) $(CFLAGS) -o appDynamic ../zad2/zad2.c -ldl -D LIB_DYNAMIC

clean:
	rm -f appStatic appShared appDynamic raport3a.txt


test:
	make clean
	make static
	make shared
	make dynamic
	@printf "Bibloteka linkowana statycznie: \n" >> raport3a.txt
	@printf "Test 1: \n" >> raport3a.txt
	@./appStatic `cat args1.txt` >> raport3a.txt
	@printf "Test 2: \n" >> raport3a.txt
	@./appStatic `cat args2.txt` >> raport3a.txt
	@printf "Test 3: \n" >> raport3a.txt
	@./appStatic `cat args3.txt` >> raport3a.txt
	@printf "Test 4: \n" >> raport3a.txt
	@./appStatic `cat args4.txt` >> raport3a.txt
	@printf "Bibloteka wspołdzielona: \n" >> raport3a.txt
	@printf "Test 1: \n" >> raport3a.txt
	@./appShared `cat args1.txt` >> raport3a.txt
	@printf "Test 2: \n" >> raport3a.txt
	@./appShared `cat args2.txt` >> raport3a.txt
	@printf "Test 3: \n" >> raport3a.txt
	@./appShared `cat args3.txt` >> raport3a.txt
	@printf "Test 4: \n" >> raport3a.txt
	@./appShared `cat args4.txt` >> raport3a.txt
	@printf "Bibloteka ładowana dynamicznie: \n" >> raport3a.txt
	@printf "Test 1: \n" >> raport3a.txt
	@./appDynamic `cat args1.txt` >> raport3a.txt
	@printf "Test 2: \n" >> raport3a.txt
	@./appDynamic `cat args2.txt` >> raport3a.txt
	@printf "Test 3: \n" >> raport3a.txt
	@./appDynamic `cat args3.txt` >> raport3a.txt
	@printf "Test 4: \n" >> raport3a.txt
	@./appDynamic `cat args4.txt` >> raport3a.txt

	@cat raport3a.txt
