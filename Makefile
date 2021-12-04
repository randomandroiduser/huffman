CC = gcc
CFLAGS = -W -Wall
EXEC = huffman

all: $(EXEC)

huffman: functions.o main.o 
	$(CC) -o huffman functions.o main.o

functions.o: functions.c functions.h
	$(CC) -o functions.o -c functions.c $(CFLAGS)

main.o: main.c functions.h
	$(CC) -o main.o -c main.c $(CFLAGS)

clean:
	rm -rf *.o

cleanOutput:
	rm -rf *.bin *Table.txt