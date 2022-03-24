CC=gcc
CFLAGS=-Wall -g

all:
	make clean
	make mainSTAT
	make mainNFTW

mainSTAT:
	$(CC) $(CFLAGS) -D STAT -o mainSTAT zad3.c

mainNFTW:
	$(CC) $(CFLAGS) -D NFTW -o mainNFTW zad3.c

clean:
	rm -f *.o main
