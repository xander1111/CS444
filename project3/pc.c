#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#include "eventbuf.h"


int prod_count;
int cons_count;
int event_create_count;
int max_events;

struct eventbuf *buffer;


void *producer(void *arg) {
    (void)arg;
    return NULL;
}

void *consumer(void *arg) {
    (void)arg;
    return NULL;
}

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

    buffer = eventbuf_create();

    pthread_t prod_threads[prod_count];
    int prod_thread_ids[prod_count];

    pthread_t cons_threads[cons_count];
    int cons_thread_ids[cons_count];

    for (int i = 0; i < prod_count; i++)
        pthread_create(prod_threads + i, NULL, producer, prod_thread_ids + i);

    for (int i = 0; i < prod_count; i++)
        pthread_create(cons_threads + i, NULL, consumer, cons_thread_ids + i);
    
    free(buffer);

    return 0;
}

