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
    printf("THREAD %lu\n", id);
    pthread_attr_getstack(&attr, &stack_addr, &stack_size);
    printf("\tStack-size: %luMB\n", stack_size / MB);
    printf("\tStack-addr: %p\n", stack_addr);

    int policy;
    struct sched_param param;
    pthread_getschedparam(id, &policy, &param);
    if (policy = SCHED_FIFO)
        printf("\tPolicy: SCHED_FIFO\n");
    else
        printf("Policy: Other Policy\n");
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

    printf("\tcan run on CPUs: |");
    for (int i = 0; i < CPU_SETSIZE; i++)   // CPU_SETSIZE = max number of CPUs
    {
        if(CPU_ISSET(i, &cpuset))   // Checks if cpu is enabled for work
            printf("%d|", i);
    }
    printf("\n");

    // Clean up
    pthread_attr_destroy(&attr);

    pthread_exit(NULL);
}

int main()
{
    // Create thread attributes
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // Store thread IDs
    pthread_t thread1, thread2, thread3;

    // Create threads
    void* out;
    int rc;

    // ------------ Thread 1 --------------

    // Attribute settings
    pthread_attr_setstacksize(&attr, 8 * MB);  // Change stack size to 8 MB

    struct sched_param param; // Change prioprity to 10
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    param.sched_priority = 1;
    pthread_attr_setschedparam(&attr, &param); 

    // Create thread
    rc = pthread_create(&thread1, &attr, thread_func, NULL);
    if (rc != 0)
        printf("Thread creation failed!\n");
    else
        pthread_join(thread1, &out);

    // ------------ Thread 2 --------------

    // Attribute settings
    pthread_attr_setstacksize(&attr, 16 * MB);  // Change stack size to 16 MB
    param.sched_priority = 2;
    pthread_attr_setschedparam(&attr, &param); 

    // Create thread
    rc = pthread_create(&thread2, &attr, thread_func, NULL);
    if (rc != 0)
        printf("Thread creation failed!\n");
    else
        pthread_join(thread2, &out);

    // ------------ Thread 3 --------------
    param.sched_priority = 3;
    pthread_attr_setschedparam(&attr, &param); 

    // Attribute settings
    void* stackaddr = malloc(24 * MB);  // Przydziel 16 MB pamięci dla stosu
    pthread_attr_setstack(&attr, stackaddr, 24 * MB);  // Ustawienie adresu i rozmiaru stosu

    // Create thread
    rc = pthread_create(&thread3, &attr, thread_func, NULL);
    if (rc != 0)
        printf("Thread creation failed!\n");
    else
        pthread_join(thread3, &out);


    // Clean thread attributes
    pthread_attr_destroy(&attr);
    

    // Wait for detached trheads to finish
    pthread_exit(NULL);
}