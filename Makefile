CC=gcc
CCOPTS= -Wall -Wextra -Werror -lpthread

.PHONY: all

all: 
	$(MAKE) -C project1 all
	$(MAKE) -C project2 all
	$(MAKE) -C project3 all
	$(MAKE) -C project5 all
	$(MAKE) -C project6 all
	$(MAKE) -C project7 all
	$(MAKE) -C project8 all

scratch: scratch.c
	$(CC) $(CCOPTS) -o $@ $<
