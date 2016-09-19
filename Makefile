SHELL := /bin/bash

CC = gcc
CFLAGS = -g --std=c99 -Wall -Wextra
LFLAGS = -lncursesw
TARGET = jmp

all:
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) *.c

clean:
	$(RM) $(TARGET)

check:
	@cppcheck -f -q .

doxygen:
	@doxygen doxygen-config

.PHONY: clean check doxygen
