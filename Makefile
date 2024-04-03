CC=gcc
CCOPTS= -Wall -Wextra -Werror

hellothread: Project1/hellothread.c
	$(CC) $(CCOPTS) -o build/$@ $<

scratch: scratch.c
	$(CC) $(CCOPTS) -o build/$@ $<
