#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    // Save current time
    struct timespec clock_end, cpu_end;
    clock_gettime(CLOCK_REALTIME, &clock_end);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu_end);

    double clock_time_end = clock_end.tv_sec + clock_end.tv_nsec / 1000000000.0;
    double CPU_time_end = cpu_end.tv_sec + cpu_end.tv_nsec / 1000000000.0;

    // Check for correct argument list
    if (argc != 3)
    {
        printf("Niepoprawna liczba argumentow\n");
        return -1;
    }

    // Read time given as arguments
    double clock_time_start = atof(argv[1]);
    double cpu_time_start = atof(argv[2]);

    // Get difference
    double elapsed_clock = clock_time_end - clock_time_start;
    double elapsed_cpu = cpu_time_start - cpu_time_start;

    // Print results
    printf("---------------------------------\n");
    printf("Czas zegara:\t%.14lf\n", elapsed_clock);
    printf("Czas CPU:\t%.14lf\n", elapsed_cpu);

}