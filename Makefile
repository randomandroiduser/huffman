CC = gcc
CFLAGS = -W -Wall
EXEC = huffman

all: $(EXEC)

huffman: compression.o decompression.o main.o 
	$(CC) -o huffman compression.o decompression.o main.o 

compression.o: compression.c compression.h
	$(CC) -o compression.o -c compression.c $(CFLAGS)

decompression.o: decompression.c decompression.h
	$(CC) -o decompression.o -c decompression.c $(CFLAGS)

main.o: main.c compression.h decompression.h
	$(CC) -o main.o -c main.c $(CFLAGS)

clean:
	rm -rf *.o