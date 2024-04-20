CC=gcc
CCOPTS= -Wall -Wextra -Werror -lpthread

.PHONY: all

all: pc

pc: pc.c eventbuf.c
	$(CC) $(CCOPTS) -o $@ $^



