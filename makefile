CC = gcc
CFLAGS = -Wall -lpthread -pthread
DEPS = defines.h

all:	park generator

park: 
	$(CC) park.c -o bin/parque $(CFLAGS)
	
generator:
	$(CC) generator.c -o bin/gerador $(CFLAGS)
