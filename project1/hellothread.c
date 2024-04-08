#include <pthread.h>
#include <stdio.h>


void *print_message(void *arg)
{
    for (int i = 0; i <= 4; i++)
    {
        printf("%s: %d\n", (char *) arg, i);
    }

    return NULL;
}

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;

    puts("Launching threads");

    pthread_t thread_1, thread_2;

    int rc =  pthread_create(&thread_1, NULL, print_message, "thread 1");
    if (rc != 0)
    {
        printf("An error while creating a thread\n");
        return 1;
    }

    rc = pthread_create(&thread_2, NULL, print_message, "thread 2");
    if (rc != 0)
    {
        printf("An error while creating a thread\n");
        return 1;
    }

    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);

    puts("Threads complete!");
    return 0;
}
