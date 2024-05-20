# CS444 VVSFS
Contains the file system project of my CS444 class. VVSFS is short for Very very simple file system

## Building

Command line:
* `make` or `make all` to build the library and test suite, produces an executable named `testfs`
* `make test` to build and run the library and test suite
* `make libvvsfs.a` to build the library by itself
* `make clean` to remove build objects
* `make pristine` to remove all build products and test files


## Files

* `testfs.c`: Main test suite
* `image.h`, `image.c`: Functions relating to op
* `block.h`, `block.c`: Functions for handling blocks
* `free.h`, `free.c`: Functions relating to the free maps
* `inode.h`, `inode.c`: Functions for handling inodes
* `ctest.h`: Testing framework, available [here](https://github.com/beejjorgensen/ctest)
* `pack.h`, `pack.c`: Functions for reading and writing integers in big endian regardless of the system. These files were provided for the project


## Data

A file (name is provided to the library, `test.txt` is used in the test suite) that is used to represent a file system.


## Functions

* `image.h`
    * `image_open()` - Opens the specified image file
    * `image_close()` - Closes the currently open image file

* `block.h`
    * `bread()` - Reads data from the currently open image into the given buffer
    * `bwrite()` - Writes data from the given buffer into the currently open image
    * `alloc()` - Allocates a data block and returns the index of the allocated block

* `free.h`
    * `set_free()` - Sets whether a certain block is marked as free or not within a free map
    * `find_free()` - Finds the first free block within a free map
    * `alloc_block()` - Finds and allocates the first free block within a free map, returns the index of the allocated block

* `inode.h`
    * `ialloc()` - Allocates an inode and returns the incore inode
    * `incore_find_free()` - Finds and returns the first free incore inode, returns NULL if there are no free incore inodes
    * `incore_find()` - Finds an incore inode with the given inode number, returns NULL if the inode number is not found incore
    * `incore_free_all()` - Frees all inodes currently incore
    * `read_inode()` - Reads the specified inode from disk and stores it as an incore inode in the `in` parameter
    * `write_inode()` - Writes the given incore inode to disk
    * `iget()` - Returns an incore version of the specified inode, if it's not already in memory it will retrieve it from disk. Also handles setting the incore inode number and updating it's reference count
    * `iput()` - Removes a reference to the given inode. If this causes the inode to be freed, it also gets saved to disk.

## Notes
None

