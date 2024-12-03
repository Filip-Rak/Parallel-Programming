#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

// Data Structs
// -------------------
typedef struct
{
  double a, dx;
} common_args;

typedef struct
{
  int start, end, stride, id;
  common_args* c_args;
} thread_args;

// Helper Functions
// -------------------

thread_args cyclic_partitioning(int i, int N, int thread_num)
{
  thread_args arg;
  arg.start = i;
  arg.end = N;
  arg.stride = thread_num;

  return arg;
}

thread_args block_partitioning(int i, int N, int thread_num)
{
  int per_thread = (N + thread_num - 1) / thread_num;
  int potential_limit = (i + 1) * per_thread;

  thread_args arg;
  arg.start = i * per_thread;
  arg.end = (potential_limit > N) ? N : potential_limit;
  arg.stride = 1;

  return arg;
}

// Declaration of integrated function
// Definition in a seperate file
double funkcja (double x);

void* calka_fragment_petli_w(void* arg)
{
  // Przeonwertuj argumenty na odpowiedni typ
  thread_args casted_arg = *(thread_args*)arg;

  int thread_id = casted_arg.id;
  double a = casted_arg.c_args->a;
  double dx = casted_arg.c_args->dx;

  // dekompozycja
  int start = casted_arg.start;
  int end = casted_arg.end;
  int stride = casted_arg.stride;

  // printf("\nWątek %d: start %d, end %d, stride %d\n", thread_id, start, end, stride);

  double* integral = malloc(sizeof(double));
  *integral = 0;
  for(int i = start; i < end; i += stride)
  {
    double x1 = a + i * dx;
    *integral += 0.5 * dx * (funkcja(x1) + funkcja(x1 + dx));
    //printf("i %d, x1 %lf, funkcja(x1) %lf, całka = %.15lf\n", i, x1, funkcja(x1), calka);

  }
  
  pthread_exit((void*)integral);
}


// Exportable Functions
// -------------------
double calka_zrownoleglenie_petli(double a, double b, double dx, int thread_num, bool cyclic)
{

  int N = ceil((b - a) / dx);
  double dx_adjust = (b - a) / N;

  // printf("Obliczona liczba trapezów: N = %d, dx_adjust = %lf\n", N, dx_adjust);
  //printf("a %lf, b %lf, n %d, dx %.12lf (dx_adjust %.12lf)\n", a, b, N, dx, dx_adjust);

  // Argumenty wspoldzielone
  common_args c_args = {a, dx_adjust};

  // Alokacja pamieci dynamicznej
  pthread_t *threads = malloc(thread_num * sizeof(pthread_t));
  thread_args *args = malloc(thread_num * sizeof(thread_args));

  // Tworzenie wątków i struktur danych
  for (int i = 0; i < thread_num; i++)
  {
      if (cyclic)
        args[i] = cyclic_partitioning(i, N, thread_num);
      else 
      {
        args[i] = block_partitioning(i, N, thread_num);
      }

      // Independent of partitioning model
      args[i].c_args = &c_args;
      args[i].id = i;

      // Make a thread
      int rc = pthread_create(&threads[i], NULL, calka_fragment_petli_w, &args[i]);
      if (rc != 0)
        printf("Thread creation failed!\n");
  }

  // Wait for threads to finish
  double sum = 0;
  for (int i = 0; i < thread_num; i++)
  {
    // Collect thread's solution
    void* buffer;
    pthread_join(threads[i], &buffer);

    // Add to the sum and free the buffer
    sum += *(double*) buffer;
    free(buffer);
  }

  // Memory cleanup
  free(threads);
  free(args);


  // Return the final result
  return sum;
}