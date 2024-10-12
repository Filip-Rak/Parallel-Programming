#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   
#include <sys/types.h> 
#include <sys/wait.h>
#include <sched.h>
#include <linux/sched.h>

#define STACK_SIZE 1024*64

int global_var = 0;

int thread_func(void* arguments)
{
    int local_var = *((int*)arguments);

    for (int i = 0; i < 10000000; i++)
    {
        global_var++;
        local_var++;
    }

    printf("----------THREAD----------\n");
    printf("global_var: %d\n", global_var);
    printf("local_var: %d\n", local_var);

    return 0;
}

int main()
{
    // Memory allocation
    void* stack1 = malloc(STACK_SIZE);
    void* stack2 = malloc(STACK_SIZE);
    if (stack1 == NULL || stack2 == NULL)
    {
        printf("Issues with stack allocation\n");
        return -1;
    }

    // Thread arguments
    int arg1 = 0;

    // Creating threads
    pid_t thread1, thread2;
    thread1 = clone(thread_func, stack1 + STACK_SIZE, CLONE_VM, &arg1);
    thread2 = clone(thread_func, stack2 + STACK_SIZE, CLONE_VM, &arg1);

    // Waiting for thread execution
    waitpid(thread1, NULL, __WCLONE);
    waitpid(thread2, NULL, __WCLONE);

    // Outputting results
    printf("----------MAIN----------\n");
    printf("global_var: %d\n", global_var);
    printf("arg: %d\n", arg1);

    // Memory cleanup
    free(stack1);
    free(stack2);

    return 0;
}