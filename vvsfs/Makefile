CC=gcc
CCOPTS= -Wall -Wextra -Werror

.PHONY: all

all: testfs.o libvvsfs.a
	$(CC) -o testfs $^

testfs.o: testfs.c ctest.h image.h block.h
	$(CC) $(CCOPTS) -DCTEST_ENABLE -c $<

libvvsfs.a: image.o block.o
	ar rcs $@ $^

image.o: image.c
	$(CC) $(CCOPTS) -c $<

block.o: block.c
	$(CC) $(CCOPTS) -c $<

clean:
	rm -f *.o

pristine: clean
	rm -f testfs *.a
