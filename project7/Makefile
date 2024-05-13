CC=gcc
CCOPTS= -Wall -Wextra -Werror

.PHONY: all, clean, pristine, test

all: testfs.o libvvsfs.a
	$(CC) -o testfs $^

test: all
	./testfs

testfs.o: testfs.c ctest.h image.h block.h free.h inode.h
	$(CC) $(CCOPTS) -DCTEST_ENABLE -c $<

libvvsfs.a: image.o block.o free.o inode.o
	ar rcs $@ $^

image.o: image.c
	$(CC) $(CCOPTS) -c $<

block.o: block.c
	$(CC) $(CCOPTS) -c $<

free.o: free.c
	$(CC) $(CCOPTS) -c $<

inode.o: inode.c
	$(CC) $(CCOPTS) -c $<

clean:
	rm -f *.o

pristine: clean
	rm -f testfs *.a
