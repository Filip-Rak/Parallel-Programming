#include <pthread.h>
#include <stdio.h>

pthread_mutex_t barrier_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t barrier_cond = PTHREAD_COND_INITIALIZER;

int total_threads = 0;
int counter = 0;

void bariera_init(int thread_num)
{
    pthread_mutex_lock(&barrier_mutex);
    total_threads = thread_num;
    counter = 0;
    pthread_mutex_unlock(&barrier_mutex);
}

void bariera()
{
    pthread_mutex_lock(&barrier_mutex);
    
    counter += 1;
    if(counter >= total_threads)
    {
        counter = 0;
        pthread_cond_broadcast(&barrier_cond);  
        printf("---------------------------\n");
    }
    else 
        pthread_cond_wait(&barrier_cond, &barrier_mutex);

    pthread_mutex_unlock(&barrier_mutex);
}