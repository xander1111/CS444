CC=gcc
CCOPTS= -Wall -Wextra -Werror -lpthread

.PHONY: all

all: project1 project2

project1: project1/hellothread.c
	$(CC) $(CCOPTS) -o build/hellothread $<

project2: project2/reservations.c
	$(CC) $(CCOPTS) -o build/reservations $<

scratch: scratch.c
	$(CC) $(CCOPTS) -o build/$@ $<
