# CS444 VVSFS
Contains the file system project of my CS444 class. VVSFS is short for Very very simple file system

## Building

Command line:
* `make` or `make all` to build the library and test suite, produces an executable named `testfs`
* `make libvvsfs.a` to build the library by itself
* `make clean` to remove build objects
* `make pristine` to remove all build products


## Files

* `testfs.c`: Main test suite
* `image.h`, `image.c`: Functions relating to op
* `ctest.h`: Testing framework, available [here](https://github.com/beejjorgensen/ctest)


## Data

A file (name is provided to the library, `test.txt` is used in the test suite) that is used to represent a file system.


## Functions

* `image_open()` - Opens the specified image file
* `image_close()` - Closes the currently open image file

* `bread()` - Reads data from the currently open image into the given buffer
* `bwrite()` - Writes data from the given buffer into the currently open image

## Notes
None

