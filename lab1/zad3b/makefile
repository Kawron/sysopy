CC=gcc
CFLAGS=-Wall

static:
	# upewniam się że jest lib
	make -C ../zad1 static_lib
	$(CC) $(CFLAGS) $(OPT) -o app ../zad2/zad2.c -L../zad1 -lzad1static

shared:
	make -C ../zad1 shared_lib
	$(CC) $(CFLAGS) $(OPT) -o app ../zad2/zad2.c -L../zad1 -lzad1shared -Wl,-rpath,../zad1

dynamic:
	make -C ../zad1 shared_lib
	$(CC) $(CFLAGS) $(OPT) -o app ../zad2/zad2.c -ldl -D LIB_DYNAMIC

clean:
	rm -f appStatic appShared appDynamic raport3b.txt

test:
	make clean

	for lib_type in static shared dynamic ; do \
		for opt in -O0 -O1 -O2 -O3 -Os ; do \
			make $$lib_type OPT=$$opt ; \
			printf "$$lib_type $$opt \n" >> raport3b.txt ; \
			make print_raport ; \
		done ; \
	done

print_raport:
	@printf "Test 1: \n" >> raport3b.txt
	@./app `cat args1.txt` >> raport3b.txt
	@printf "Test 2: \n" >> raport3b.txt
	@./app `cat args2.txt` >> raport3b.txt
	@printf "Test 3: \n" >> raport3b.txt
	@./app `cat args3.txt` >> raport3b.txt
	@printf "Test 4: \n" >> raport3b.txt
	@./app `cat args4.txt` >> raport3b.txt

	@cat raport3b.txt
