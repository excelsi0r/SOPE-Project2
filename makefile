CC = gcc
CFLAGS = -Wall -lpthread -pthread
DEPS = defines.h

all:	clean park generator

park: 
	$(CC) park.c -o bin/park $(CFLAGS)
	
generator:
	$(CC) generator.c -o bin/generator $(CFLAGS)
	
clean:
	rm bin/*
