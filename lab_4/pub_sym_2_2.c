// ----------------------------------------------------------
// ---------------------- Exercise --------------------------
// Simulate a pub with clients being threads.----------------
// With unique limited beer mugs and unique limited beer taps
// ----------------------------------------------------------
// Execution arguments: (name), client_number, mug_number ---
// ----------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

//--------------------
// Globals

// Consts
#define ARG_NUMBER 3
#define TAP_NUMBER 4
#define DRINKING_LIMIT_MAX 3
#define TAP_NAME_LENGTH 50
#define MUG_FILL_TIME 25
#define MUG_DRINK_TIME 50

// Shared by threads
pthread_mutex_t mug_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tap_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct 
{
    char name[TAP_NAME_LENGTH];
    int fills;
    bool is_free;
} Beer_Tap;

typedef struct 
{
    int picks;
    bool is_free;
} Beer_Mug;

Beer_Tap beer_taps[TAP_NUMBER] = {{"Perla", 0, true}, {"Zywiec", 0, true}, {"Heineken", 0, true}, {"Water (98%)", 0, true}};

Beer_Mug* beer_mug;
int mug_number;

//--------------------
// Functions
void* thread_func(void* arg);

int wait_for_a_mug(int* done_work);
int wait_for_a_tap(int* done_work);
void wait_to_return_tap(int used_tap_index, int* done_work);
void wait_to_return_mug(int used_mug_index, int* done_work);

int main(int argc, char* argv[])
{
    // Check if the number of arguments is correct
    if (argc != ARG_NUMBER)
    {
        printf("Invalid number of arguments! %d instead of %d\n", argc, ARG_NUMBER);
        return -1;
    }

    // Cast given arguments to integers
    int client_number = atoi(argv[1]);
    mug_number = atoi(argv[2]);

    // Check if the conversion was succesful
    if (client_number == 0 || mug_number == 0)
    {
        printf("Invalid argument types! Should be integers\n");
        return -2;
    }

    // Allocate dynamic memory
    beer_mug = malloc(mug_number * sizeof(Beer_Mug));
    pthread_t *threads = malloc(client_number * sizeof(pthread_t));
    int *thread_ids = malloc(client_number * sizeof(int));
    int *done_work = malloc(client_number * sizeof(int));

    // Initialize dynamic memory
    for(int i = 0; i < mug_number; i++)
    {
        beer_mug[i].picks = 0;
        beer_mug[i].is_free = true;
    }

    // Create parallel threads representing clients
    for(int i = 0; i < client_number; i++)
    {
        thread_ids[i] = i;
        int rc = pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
        if (rc != 0)
            printf("Failed to create thread %d. Reason: %d", i, rc);
    }

    // Get done work from threads
    for(int i = 0; i < client_number; i++)
    {
        if(threads[i])
        {
            // Store thread's work in a buffer for easy casting
            void* buffer;
            pthread_join(threads[i], &buffer);

            // Cast the return to an integer
            done_work[i] = *(int*)(buffer);

            // Free the buffer
            free(buffer);
        }
        else 
            done_work[i] = 0;
    }

    // Output work done by the threads
    printf("\n---------- Summarization of work done by each thread ----------\n");
    for(int i = 0; i < client_number; i++)
    {
        printf("Thread: %d\tWork done: %d\n", i, done_work[i]);
    }

    // Output uses of each tap
    printf("\n---------- Summarization of fills from each tap ---------------\n");
    for(int i = 0; i < TAP_NUMBER; i++)
        printf("Tap: %-15s\tFills: %d\tIs free: %d\n", beer_taps[i].name, beer_taps[i].fills, beer_taps[i].is_free);

    // Output uses of each mug
    printf("\n---------- Summarization of picks of each mug -----------------\n");
    for(int i = 0; i < mug_number; i++)
        printf("Mug: %-15d\tPicks: %d\tIs free: %d\n", i, beer_mug[i].picks, beer_mug[i].is_free);

    // Clear dynamically allocated memory
    free(beer_mug);
    free(threads);
    free(thread_ids);
    free(done_work);

    pthread_exit(NULL);
}

void* thread_func(void* arg)
{
    // Cast the argument to a thread_id
    int thread_id = *(int*)arg;
    
    // Save a localized drinking limit
    int drinking_limit_left = DRINKING_LIMIT_MAX;

    // Variable to represent work a thread can do while waiting
    // Allocate dynamically to return later
    int *done_work = malloc(sizeof(int));
    *done_work = 0;

    // Announce the entrance
    printf("Client %d entered the pub\n", thread_id);

    while(drinking_limit_left > 0)
    {
        // Try getting a mug
        int my_mug_index = wait_for_a_mug(done_work);
        printf("Client %d has taken a mug number: %d\n", thread_id, my_mug_index);

        // Try getting to a tap 
        int used_tap_index = wait_for_a_tap(done_work);
        printf("Client %d is filling the mug with: %s\n", thread_id, beer_taps[used_tap_index].name);

        // Take time to fill the tap
        usleep(MUG_FILL_TIME);

        // Return the tap
        wait_to_return_tap(used_tap_index, done_work);
        printf("Client %d has left the tap\n", thread_id);

        // Take time to drink the mug contents
        printf("Client %d is drinking %s from mug %d\n", thread_id, beer_taps[used_tap_index].name, my_mug_index);
        usleep(MUG_DRINK_TIME);

        // Return the mug to the pool
        wait_to_return_mug(my_mug_index, done_work);
        printf("Client %d has returned the mug\n", thread_id);

        // Decrease drinking limit
        drinking_limit_left -= 1;
    }

    pthread_exit((void*) done_work);
}

int wait_for_a_mug(int* done_work)
{
    int mug_index = -1;
    while(true)
    {
        if(pthread_mutex_trylock(&mug_mutex) == 0)
        {
            // Find a mug that is available
            for(int i = 0; i < mug_number; i++)
            {
                if (beer_mug[i].is_free == true)
                {
                    // Reserve the mug
                    beer_mug[i].is_free = false;
                    beer_mug[i].picks += 1;
                    mug_index = i;
                    break;  // Break inner loop
                }
            }

            pthread_mutex_unlock(&mug_mutex);

            // Return if mug is found
            if(mug_index != -1)
                return mug_index;
        }
        else 
        {
            *done_work += 1;
        }            
    }
}

int wait_for_a_tap(int* done_work)
{
    int tap_index = -1;
    while(true)
    {
        if(pthread_mutex_trylock(&tap_mutex) == 0)
        {
            // Find a tap that is available
            for(int i = 0; i < TAP_NUMBER; i++)
            {
                if(beer_taps[i].is_free)
                {
                    // Reserve the tap
                    beer_taps[i].is_free = false;
                    beer_taps[i].fills += 1;
                    tap_index = i;
                    break;  // Break the inner loop
                }
            }

            pthread_mutex_unlock(&tap_mutex);

            // Return the tap index if found
            if(tap_index != -1)
                return tap_index;
        }
        else 
        {
            *done_work += 1;
        }            
    }
}

void wait_to_return_tap(int used_tap_index, int* done_work)
{
    while(true)
    {
        if(pthread_mutex_trylock(&tap_mutex) == 0)
        {
            // When possible return the tap
            beer_taps[used_tap_index].is_free = true;
            pthread_mutex_unlock(&tap_mutex);

            // Exit the function
            return;
        }
        else 
        {
            *done_work += 1;
        }            
    }
}

void wait_to_return_mug(int used_mug_index, int* done_work)
{
    while(true)
    {
        if(pthread_mutex_trylock(&mug_mutex) == 0)
        {
            // When possible return the mug
            beer_mug[used_mug_index].is_free = true;
            pthread_mutex_unlock(&mug_mutex);

            // Exit the function
            return;
        }
        else 
        {
            *done_work += 1;
        }            
    }
}