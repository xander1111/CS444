#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#include "eventbuf.h"


int prod_count;
int cons_count;
int event_create_count;
int max_events;


int main(int argc, char const *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "usage: pc prod_count cons_count event_create_count max_events\n");
        exit(1);
    }

    prod_count = atoi(argv[1]);
    cons_count = atoi(argv[2]);
    event_create_count = atoi(argv[3]);
    max_events = atoi(argv[4]);


    return 0;
}

