# CS444 Project3
Contains project 3 of my CS444 class

## Building

Command line:
* `make` or `make all` to build
* Produces an executable named `pc`


## Files

* `pc.c`: Main code for project 3
* `eventbuf.h`, `eventbuf.c`: A FIFO queue that was provided for the project


## Data

Includes an 'event buffer', a FIFO queue that events get placed in an pulled from.


## Functions

* `main()`
    * `producer()` - Produces events and places them in the event buffer
    * `consumer()` - Pulls events from the event buffer and consumes them
* `sem_open_temp()` - A helper function that creates a semaphore, was provided for the project

## Notes
None

