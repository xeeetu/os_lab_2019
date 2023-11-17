CC=gcc
CFLAGS=-I. -std=c99 -pthread

all: mutex no_mutex factorial deadlock

again: clean all

mutex: mutex.c
	$(CC) -o mutex mutex.c $(CFLAGS)

no_mutex: no_mutex.c
	$(CC) -o no_mutex no_mutex.c $(CFLAGS)

factorial: factorial.c
	$(CC) -o factorial factorial.c $(CFLAGS)

deadlock: deadlock.c
	$(CC) -o deadlock deadlock.c $(CFLAGS)

clean :
	rm mutex no_mutex factorial deadlock