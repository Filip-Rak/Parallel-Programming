// -------------------------------------------------------------
// ---------------------- Exercise -----------------------------
// Simulate a pub with clients being threads.-------------------
// With unique limited beer mugs and unique limited beer taps.--
// Use trylock and busy-waiting to show how much work a thread--
// could have done in the meantime. ----------------------------
// -------------------------------------------------------------
// Execution arguments: (name), client_number, mug_number ------
// -------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

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
typedef struct 
{
    char name[TAP_NAME_LENGTH];
    int fills;
    pthread_mutex_t mutex;
} Beer_Tap;

typedef struct 
{
    int picks;
    pthread_mutex_t mutex;
} Beer_Mug;

Beer_Tap beer_taps[TAP_NUMBER] = {{"Perla", 0, PTHREAD_MUTEX_INITIALIZER}, {"Zywiec", 0, PTHREAD_MUTEX_INITIALIZER}, 
    {"Heineken", 0, PTHREAD_MUTEX_INITIALIZER}, {"Water (96%)", 0, PTHREAD_MUTEX_INITIALIZER}};

Beer_Mug* beer_mug;
int mug_number;

//--------------------
// Functions

// Main thread function
void* thread_func(void* arg);

// Thread helper functions
int wait_for_a_mug(int* done_work);
int wait_for_a_tap(int* done_work);
void wait_to_return_tap(int used_tap_index, int* done_work);
void wait_to_return_mug(int used_mug_index, int* done_work);

// Main function
int main(int argc, char* argv[])
{
    // Check if the number of arguments is correct
    if (argc != ARG_NUMBER)
    {
        printf("ERROR: Invalid number of arguments! %d instead of %d\n", argc, ARG_NUMBER);
        return -1;
    }

    // Cast given arguments to integers
    int client_number = atoi(argv[1]);
    mug_number = atoi(argv[2]);

    // Check if the conversion was succesful
    if (client_number == 0 || mug_number == 0)
    {
        printf("ERROR: Invalid argument types! Should be integers\n");
        return -2;
    }

    // Allocate dynamic memory
    beer_mug = malloc(mug_number * sizeof(Beer_Mug));

    pthread_t *threads = malloc(client_number * sizeof(pthread_t));
    bool* thread_created = malloc(client_number * sizeof(bool));
    int *thread_ids = malloc(client_number * sizeof(int));
    int *done_work = malloc(client_number * sizeof(int));

    // Verify if memory allocation was successful
    if(beer_mug == NULL || threads == NULL || thread_created == NULL || thread_ids == NULL || done_work == NULL)
    {
        printf("ERROR: Failed to allocate memory! Exitting...\n");

        // Clear dynamically allocated memory
        free(beer_mug);
        free(threads);
        free(thread_created);
        free(thread_ids);
        free(done_work);

        return -3;
    }

    // Initialize dynamic memory
    for(int i = 0; i < mug_number; i++)
    {
        beer_mug[i].picks = 0;
        pthread_mutex_init(&beer_mug[i].mutex, NULL);
    }

    // Create parallel threads representing clients
    int total_created = 0;
    for(int i = 0; i < client_number; i++)
    {
        thread_ids[i] = i;
        int rc = pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
        if (rc != 0)
        {
            printf("ERROR: Failed to create thread %d. Reason: %d\n", i, rc);
            thread_created[i] = false;
        }
        else
        {
            thread_created[i] = true;
            total_created += 1;
        }
    }

    // Get work done by the threads
    for(int i = 0; i < client_number; i++)
    {
        if(thread_created[i])
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
        printf("Thread: %d\tWork done: %d\n", i, done_work[i]);

    // Output uses of each tap
    printf("\n---------- Summarization of fills from each tap ---------------\n");
    for(int i = 0; i < TAP_NUMBER; i++)
    {
        // Check if the mutex is unlocked as it should be
        char mutex_state[10] = "Locked";
        if (pthread_mutex_trylock(&beer_taps[i].mutex) == 0)
        {
            strcpy(mutex_state, "Unlocked");
            pthread_mutex_unlock(&beer_taps[i].mutex);
        }

        printf("Tap: %-15s\tFills: %d\tMutex state: %s\n", beer_taps[i].name, beer_taps[i].fills, mutex_state);
    }

    // Output uses of each mug
    printf("\n---------- Summarization of picks per mug ---------------------\n");
    for(int i = 0; i < mug_number; i++)
    {
        // Check if the mutex is unlocked as it should be
        char mutex_state[10] = "Locked";
        if (pthread_mutex_trylock(&beer_mug[i].mutex) == 0)
        {
            strcpy(mutex_state, "Unlocked");
            pthread_mutex_unlock(&beer_mug[i].mutex);
        }

        printf("Mug: %-15d\tPicks: %d\tMutex state: %s\n", i, beer_mug[i].picks, mutex_state);
    }

    printf("\n---------------------- Miscellaneous --------------------------\n");
    printf("Total threads created: %d\t Failed to create: %d\n", total_created, client_number - total_created);
    

    // Clean mutexes
    for(int i = 0; i < mug_number; i++)
        pthread_mutex_destroy(&beer_mug[i].mutex);

    for(int i = 0; i < TAP_NUMBER; i++)
        pthread_mutex_destroy(&beer_taps[i].mutex);

    // Clear dynamically allocated memory
    free(beer_mug);
    free(threads);
    free(thread_created);
    free(thread_ids);
    free(done_work);

    // Exit the main function
    return 0;
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

        // Leave the tap (unlock the mutex)
        pthread_mutex_unlock(&beer_taps[used_tap_index].mutex);
        printf("Client %d has left the tap\n", thread_id);

        // Take time to drink the mug contents
        printf("Client %d is drinking %s from mug %d\n", thread_id, beer_taps[used_tap_index].name, my_mug_index);
        usleep(MUG_DRINK_TIME);

        // Return the mug to the pool (unlock the mutex)
        pthread_mutex_unlock(&beer_mug[my_mug_index].mutex);
        printf("Client %d has returned the mug\n", thread_id);

        // Decrease drinking limit
        drinking_limit_left -= 1;
    }

    pthread_exit((void*) done_work);
}

int wait_for_a_mug(int* done_work)
{
    int i = 0;
    while(true)
    {
        if(pthread_mutex_trylock(&beer_mug[i].mutex) == 0)
        {
            // Reserve the mug
            beer_mug[i].picks += 1;

            // Return the index of the mug
            return i;
        }
        else 
        {
            // Do work in the meantime
            *done_work += 1;

            // Change the index to try another mug
            i += 1;
            i %= mug_number;
        }
    }
}

int wait_for_a_tap(int* done_work)
{
    int i = 0;
    while(true)
    {
        if(pthread_mutex_trylock(&beer_taps[i].mutex) == 0)
        {
            // Reserve the tap
            beer_taps[i].fills += 1;

            // Return the index of the tap
            return i;
        }
        else 
        {
            // Do work in the meantime
            *done_work += 1;

            // Change the index to try another tap
            i += 1;
            i %= TAP_NUMBER;
        }
    }
}
