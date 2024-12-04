#include <pthread.h>
#include <stdio.h>

typedef struct
{
    int threads;
    int counter;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
}Barrier;

void bariera_init(Barrier* barrier, int thread_num)
{
    barrier->threads = thread_num;
    barrier->counter = 0;
    pthread_cond_init(&barrier->condition, NULL);
    pthread_mutex_init(&barrier->mutex, NULL);
}

void bariera(Barrier* barrier)
{
    pthread_mutex_lock(&barrier->mutex);
    
    barrier->counter += 1;
    if(barrier->counter >= barrier->threads)
    {
        barrier->counter = 0;
        pthread_cond_broadcast(&barrier->condition);  
        // printf("---------------------------\n");
    }
    else 
        pthread_cond_wait(&barrier->condition, &barrier->mutex);

    pthread_mutex_unlock(&barrier->mutex);
}

void barrier_destroy(Barrier* barrier) 
{
    pthread_mutex_destroy(&barrier->mutex);
    pthread_cond_destroy(&barrier->condition);
}