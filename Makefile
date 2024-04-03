CC=gcc
CCOPTS= -Wall -Wextra -Werror

project1: Project1/hellothread.c
	$(CC) $(CCOPTS) -o build/hellothread $<

scratch: scratch.c
	$(CC) $(CCOPTS) -o build/$@ $<
