#include <pthread.h>
#include <stdio.h>


void *print_message(void *arg)
{
    char* message = arg;
    for (int i = 1; i <= 4; i++)
    {
        printf("%s: %d\n", message, i);
    }

    return NULL;
}

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;

    puts("Launching threads");

    pthread_t thread_1;
    pthread_create(&thread_1, NULL, print_message, "thread 1");

    pthread_t thread_2;
    pthread_create(&thread_2, NULL, print_message, "thread 2");

    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);

    puts("Threads complete!");
    return 0;
}
