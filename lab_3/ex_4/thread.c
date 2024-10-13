// ------------- EXERCISE -------------------
// Test various pthread attributes ----------
// Print these attributes from within threads
// ------------------------------------------

#define _GNU_SOURCE 
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define MB 1048576

void* thread_func(void* arg)
{
    pthread_t id = pthread_self();
    pthread_attr_t attr;
    pthread_getattr_np(id, &attr);

    void* stack_addr;
    size_t stack_size;
    printf("THREAD: %lu\n", id);
    pthread_attr_getstack(&attr, &stack_addr, &stack_size);
    printf("\tStack-size: %luMB\n", stack_size / MB);
    printf("\tStack-addr: %p\n", stack_addr);

    int policy;
    struct sched_param param;
    pthread_getschedparam(id, &policy, &param);
    if (policy == SCHED_FIFO)
        printf("\tPolicy: SCHED_FIFO\n");
    else
        printf("\tPolicy: Unknown Policy\n");
    printf("\tPriority: %d\n", param.sched_priority);

    int detach_state;
    pthread_attr_getdetachstate(&attr, &detach_state);
    if (detach_state == PTHREAD_CREATE_DETACHED)
        printf("\tDetach state: PTHREAD_CREATE_DETACHED\n");
    else if(detach_state == PTHREAD_CREATE_JOINABLE)
        printf("\tDetach state: PTHREAD_CREATE_JOINABLE\n");

    cpu_set_t cpuset;   // Struct representing cpu set
    CPU_ZERO(&cpuset);  // Struct initilization
    pthread_getaffinity_np(id, sizeof(cpuset), &cpuset);    // Get affinity mask

    printf("\tCan run on CPUs: |");
    for (int i = 0; i < CPU_SETSIZE; i++)   // CPU_SETSIZE = max number of CPUs
    {
        if(CPU_ISSET(i, &cpuset))   // Checks if cpu is enabled for work
            printf("%d|", i);
    }
    printf("\n");

    // Clean up
    pthread_attr_destroy(&attr);

    // End the thread
    pthread_exit(NULL);
}

void* ma_thread_func(void* arg)
{
    int tab[10000000] = {0}; // Total size: 40 000 000 bits

    printf("Thread %lu\n", pthread_self());
    printf("\tMemory allocation succeded\n");

    pthread_exit(NULL);
}

int main()
{
    // Thread attributes
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    void* out;
    cpu_set_t cpuset;

    // Store thread IDs
    pthread_t thread1, thread2, thread3;
    int rc;

    // ------------ Thread 1 --------------

    // Attribute settings
    // Set stack
    pthread_attr_setstacksize(&attr, 8 * MB);  // Change stack size to 8 MB

    // Set cpu mask
    CPU_ZERO(&cpuset); // Zero the mask
    CPU_SET(0, &cpuset);    // Add CPUs
    CPU_SET(1, &cpuset); 

    // Set policy and priority
    struct sched_param param; // Change prioprity to 10 (may not work on all systems)
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    param.sched_priority = 1;
    pthread_attr_setschedparam(&attr, &param); 

    // Create thread
    rc = pthread_create(&thread1, &attr, thread_func, NULL);
    if (rc != 0)
        printf("Thread creation failed!\n");
    else
    {
        pthread_setaffinity_np(thread1, sizeof(cpu_set_t), &cpuset);
        pthread_join(thread1, &out);
    }

    // ------------ Thread 2 --------------

    // Attribute settings
    // Set stack
    void* stackaddr = malloc(24 * MB);  // Chnage address and size
    pthread_attr_setstack(&attr, stackaddr, 24 * MB);

    // Set cpu mask
    CPU_ZERO(&cpuset); // Zero the mask
    CPU_SET(2, &cpuset);    // Add CPUs
    CPU_SET(4, &cpuset); 

    // Set policy and priority
    param.sched_priority = 2;
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    pthread_attr_setschedparam(&attr, &param); 

    // Create thread
    rc = pthread_create(&thread2, &attr, thread_func, NULL);
    if (rc != 0)
        printf("Thread creation failed!\n");
    else
    {
        pthread_setaffinity_np(thread2, sizeof(cpu_set_t), &cpuset);
        pthread_join(thread2, &out);
    }

    // ------------ Thread 3 --------------

    // Attribute settings

    // Reset attributes to get rid of custom stack
    pthread_attr_init(&attr);

    // Set stack
    pthread_attr_setstacksize(&attr, 16 * MB);  // Change stack size to 16 MB

    // Set cpu mask
    CPU_ZERO(&cpuset); // Zero the mask 
    CPU_SET(2, &cpuset); // Add CPUs
    CPU_SET(3, &cpuset); 

    // Set policy and priority
    pthread_attr_setschedpolicy(&attr, SCHED_DEADLINE);
    param.sched_priority = 3;
    pthread_attr_setschedparam(&attr, &param); 

    // Create thread
    rc = pthread_create(&thread3, &attr, thread_func, NULL);
    if (rc != 0)
        printf("Thread creation failed!\n");
    else
    {
        pthread_setaffinity_np(thread3, sizeof(cpu_set_t), &cpuset);
        pthread_detach(thread3);
    }

    // ------------ Thread 4 --------------
    // ----- Memory allocation test -------

    pthread_t ma_thread;

    // Add custom stack size
    pthread_attr_init(&attr); // Fully reset attributes
    pthread_attr_setstacksize(&attr, 38.154 * MB);  // Lowest succesful: 38.154 MB. | 40 000 000 bits / MB = 38.15

    // Create the thread
    rc = pthread_create(&ma_thread, &attr, ma_thread_func, NULL);
    if (rc != 0)
        printf ("Creation of Thread 4 failed! Reason: %d\n", rc);
    else 
        pthread_join(ma_thread, &out);

    // Clean up
    pthread_attr_destroy(&attr);
    free(stackaddr);

    // Wait for detached threads to finish
    pthread_exit(NULL);
}