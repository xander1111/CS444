CC=gcc
CCOPTS= -Wall -Wextra -Werror

.PHONY: all

all: testfs.o image.o
	$(CC) -o testfs $^

testfs.o: testfs.c ctest.h image.h
	$(CC) $(CCOPTS) -DCTEST_ENABLE -c $^

image.o: image.c
	$(CC) $(CCOPTS) -c $^

clean:
	rm -f *.o *.a

pristine: clean
	rm -f testfs
