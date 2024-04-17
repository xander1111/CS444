# CS444 Project2
Contains project 2 of my CS444 class

## Building

Command line:
* `make` or `make all` to build
* Produces an executable named `reservations`


## Files

* `reservations.c`: Main code for project 2


## Data

Includes an array of booleans/ints, the size of which is determined by command line arguments. This array stores whether a seat is taken or not; `true` if a seat is taken, `false` if it's not.


## Functions

* `main()`
    * `is_free()` - Checks whether a seat is free or not, not thread safe
    * `reserve_seat()` - Marks a seat as reserved, thread safe
    * `free_seat()` - Marks a seat as free, thread safe
    * `verify_seat_count()` - Checks whether the number of seats reserved in the seat array matches the number of seats claimed by brokers
    * `seat_broker()` - Runs a seat broker; randomly reserves and frees seats.

## Notes
None

