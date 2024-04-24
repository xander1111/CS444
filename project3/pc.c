#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>

#include "eventbuf.h"


int prod_count;
int cons_count;
int event_create_count;
int max_events;

struct eventbuf *buffer;
sem_t *buffer_lock;

sem_t *prod_sem;
sem_t *cons_sem;


sem_t *sem_open_temp(const char *name, unsigned int value)
{
    sem_t *sem;

    // Create the semaphore
    if ((sem = sem_open(name, O_CREAT, 0600, value)) == SEM_FAILED)
        return SEM_FAILED;

    // Unlink it so it will go away after this process exits
    if (sem_unlink(name) == -1) {
        sem_close(sem);
        return SEM_FAILED;
    }

    return sem;
}


void *producer(void *arg) {
    int id = *((int*)arg);

    for (int i = 0; i < event_create_count; i++)
    {
        sem_wait(prod_sem);  // Check if there is space in the buffer

        int event_id = id * 100 + i;
        printf("P%d: adding event %d\n", id, event_id);

        sem_wait(buffer_lock);
        eventbuf_add(buffer, event_id);
        sem_post(buffer_lock);

        sem_post(cons_sem);
    }

    printf("P%d: exiting\n", id);
    
    return NULL;
}

void *consumer(void *arg) {
    int id = *((int*)arg);
    int event_id;

    while (1)
    {
        sem_wait(cons_sem);  // Check if there are items in the buffer to be consumed

        if (eventbuf_empty(buffer))
            break;  // The event buffer will only ever be empty at this point if something wasn't added when
                    // the semaphore was posted to, i.e. the main thread signaled to exit

        sem_wait(buffer_lock);
        event_id = eventbuf_get(buffer);
        sem_post(buffer_lock);

        printf("C%d: got event %d\n", id, event_id);
        sem_post(prod_sem);
    }

    printf("C%d: exiting\n", id);

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
    buffer_lock = sem_open_temp("buffer_lock", 1);

    prod_sem = sem_open_temp("producer_semaphore", max_events);
    cons_sem = sem_open_temp("consumer_semaphore", 0);

    pthread_t prod_threads[prod_count];
    int prod_thread_ids[prod_count];

    pthread_t cons_threads[cons_count];
    int cons_thread_ids[cons_count];

    for (int i = 0; i < prod_count; i++)
    {
        prod_thread_ids[i] = i;
        pthread_create(prod_threads + i, NULL, producer, &prod_thread_ids[i]);
    }

    for (int i = 0; i < cons_count; i++)
    {
        cons_thread_ids[i] = i;
        pthread_create(cons_threads + i, NULL, consumer, &cons_thread_ids[i]);
    }


    for (int i = 0; i < prod_count; i++)
        pthread_join(*(prod_threads + i), NULL);

    for (int i = 0; i < cons_count; i++)
        sem_post(cons_sem);

    for (int i = 0; i < cons_count; i++)
        pthread_join(*(cons_threads + i), NULL);
    
    eventbuf_free(buffer);

    return 0;
}

