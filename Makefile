CC=gcc
CCOPTS= -Wall -Wextra -Werror -lpthread

.PHONY: all

all: 
	$(MAKE) -C project1 all
	$(MAKE) -C project2 all
	$(MAKE) -C project3 all
	$(MAKE) -C project5 all

scratch: scratch.c
	$(CC) $(CCOPTS) -o $@ $<
