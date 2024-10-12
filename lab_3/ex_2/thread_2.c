// ------------- EXERCISE -----------------
// Create threads within a loop -----------
// Display their custom and system ids ----
// ----------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void* thread_func(void* arg)
{
    int* custom_id = (int*)arg;
    printf("THREAD -> \tCustom id: %d\tSystem_id: %lu\n", *custom_id, pthread_self());

    // Each thread cleans it's own ID from the memory
    free(custom_id);

    // Exit the thread with null
    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    // Load in the number of threads to create
    if (argc != 2)
    {
        printf("MAIN::Invalid number of args! %d\n", argc);
        return -1;
    }

    int thread_number = atoi(argv[1]);
    if (thread_number == 0)
    {
        printf("MAIN::Invalid args!\n");
        return -2;
    }

    // Set up attributes
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // Create threads
    for (int i = 0; i < thread_number; i++)
    {
        // Allocate memory for a custom id of each thread
        pthread_t thread;   // Irrelevant, will instantly detach
        int *id = malloc(sizeof(int)); // Will be freed by a thread
        *id = i;

        int rc = pthread_create (&thread, &attr, thread_func, id);
        if (rc != 0)
        {
            printf("ERROR: Creation of thread %d failed, reason: %d\n", i, rc);
            free(id);
        }

    }

    // Clean up after attributes
    pthread_attr_destroy(&attr);

    // End the main thread, without stopping detached threads
    pthread_exit(NULL);
}