// ------------- EXERCISE -----------------
// Create a struct for arguments ----------
// Pass the structs to the threads --------
// ----------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

struct thread_args
{
    int id;
    double x1, y1;
    double x2, y2;
    double result;
};

void* thread_func(void* args)
{
    // Cast given argument to the correct format
    struct thread_args *arg = (struct thread_args*)args;

    // Do an example computaion 
    double x = arg->x1 * arg->x2 * -1.0;
    double y = arg->y1 * arg->y2 * 0.1;

    arg->result = x + y;
    // arg->result = 1; // More predictible result
    printf("THREAD %d:\tComputation finished\tResult: %lf\n", arg->id, arg->result);
    
    // End the work of the thread
    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    // Load in the number of threads to create
    if (argc != 2)
    {
        printf("ERROR: Invalid number of args! %d\n", argc);
        return -1;
    }

    int thread_number = atoi(argv[1]);
    if (thread_number == 0)
    {
        printf("Error: Invalid args!\n");
        return -2;
    }

    // Allocate memory for thread ids and args
    pthread_t *threads = malloc(sizeof(pthread_t) * thread_number);
    struct thread_args *args = malloc(sizeof(struct thread_args) * thread_number);

    if (threads == NULL || args == NULL)
    {
        printf("ERROR: Memory allocation failed! Exitting...\n");
        return 0;
    }

    // Create threads
    for(int i = 0; i < thread_number; i++)
    {
        // Fill the structs with example values
        args[i].id = i;
        args[i].x1 = i / 16.0;
        args[i].x2 = i / 2.0;
        args[i].y1 = i / 4.0;
        args[i].y2 = i * 2.0;
        args[i].result = 0.0;   // Initialization for safety

        int rc = pthread_create(&threads[i], NULL, thread_func, &args[i]);
        if (rc != 0)
            printf("ERROR: Creation of thread %d failed! Reason: %d\n", i, rc);
    }

    // Wait for threads to finish and gather results
    double sum = 0;
    for (int i = 0; i < thread_number; i++)
    {
        if(pthread_join(threads[i], NULL) != 0)
            printf("ERROR: Issues occured with thread %d!\n", i);

        sum += args[i].result;
    }

    printf("MAIN -> Sum of all results: %lf\n", sum);

    // Release allocated memory
    free(threads);
    free(args);

    return 0;
}