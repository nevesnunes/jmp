SHELL := /bin/bash

CC = gcc
CFLAGS = -g --std=c99 -Wall -Wextra
LFLAGS = -lncursesw
TARGET = jmp

all:
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) *.c

clean:
	$(RM) $(TARGET) *.o *.so

check:
	@cppcheck -f -q .

doxygen:
	@doxygen doxygen-config

libraries:
	$(CC) $(CFLAGS) -fPIC -c all.c hints.c
	$(CC) $(CFLAGS) -shared -o libhints.so all.o hints.o

.PHONY: clean check doxygen
