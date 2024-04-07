CC=gcc
CCOPTS= -Wall -Wextra -Werror -lpthread

all:
	make project1


project1: project1/hellothread.c
	$(CC) $(CCOPTS) -o build/hellothread $<

scratch: scratch.c
	$(CC) $(CCOPTS) -o build/$@ $<
