#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

// Data Structs
// -------------------
typedef struct
{
    double a, b, dx;
    int id;
}
thread_args;

// Helper Functions
// -------------------
double function (double x);

double calka_sekw(double a, double b, double dx);

void* thread_func(void* arg)
{
    // Cast given arguments
    thread_args args = *((thread_args*)arg);

    // Find integral ina given range
    double *integral = malloc(sizeof(double));
    *integral = calka_sekw(args.a, args.b, args.dx);

    // Return result as a pointer
    pthread_exit(integral);
}

// Exportable Functions
// -------------------
double calka_dekompozycja_obszaru(double a, double b, double dx, int thread_num)
{
    // Dynamically allocate memory
    pthread_t *thread_arr = malloc(thread_num * sizeof(pthread_t));
    thread_args *args_arr = malloc(thread_num * sizeof(thread_args));
    bool *thread_created = malloc(thread_num * sizeof(bool));

    if (thread_arr == NULL || args_arr == NULL | thread_created == NULL)
    {
        printf("ERROR: Memory allocation failure! Suspending...\n");
        
        // Free resources
        free(thread_arr);
        free(args_arr);
        free(thread_created);

        return -1;
    }

    // Create threads
    for (int i = 0; i < thread_num; i++)
    {
        // Get range for each thread
        double range_per_thread = (b - a) / thread_num;

        args_arr[i].a = a + i * range_per_thread;
        args_arr[i].b = a + (i + 1) * range_per_thread;
        args_arr[i].dx = dx;
        args_arr[i].id = i;

        // Create a thread
        int rc = pthread_create(&thread_arr[i], NULL, thread_func, &args_arr[i]);
        if (rc != 0)
        {
            printf("ERROR: Failed to create thread: %d\n!", i);
            thread_created[i] = false;
        }
        else 
            thread_created[i] = true;
    }

    // Collect each thread's solution
    double integral = 0;
    for (int i = 0; i < thread_num; i++)
    {
        // Avoid waiting for non existent threads
        if (!thread_created[i])
            continue;

        // Collect result and store it in a buffer
        void* buffer;
        int rc = pthread_join(thread_arr[i], &buffer);
        if (rc != 0)
            printf("ERROR: Join failed on thread: %d! Part of the solution has been lost.\n", i);

        // Add casted value and free the buffer
        integral += *(double*)buffer;
        free(buffer);
    }
    
    // Free resources
    free(thread_arr);
    free(args_arr);
    free(thread_created);

    // Return integral's estimated value
    return integral;
}

