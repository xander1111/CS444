CC=gcc
CCOPTS= -Wall -Wextra -Werror -lpthread

.PHONY: all

all: testfs.o
	$(CC) -o testfs $^

testfs.o: testfs.c ctest.h
	$(CC) $(CCOPTS) -DCTEST_ENABLE -c $^

clean:
	rm -f *.o *.a

pristine: clean
	rm -f testfs
