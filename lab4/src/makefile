CC=gcc
CFLAGS=-I. -std=c99
THFLAGS=-pthread -lsum -L.

all: parallel_min_max process_memory

again: clean all

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

process_memory:
	$(CC) -o process_memory process_memory.c $(CFLAGS)

clean :
	rm utils.o find_min_max.o parallel_min_max process_memory
