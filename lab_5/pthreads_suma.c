#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>

#include "pomiar_czasu.h"

#define SIZE 1000000
#define THREAD_NUM 2

pthread_mutex_t muteks; 
pthread_t threads[THREAD_NUM];
double global_array_of_local_sums[THREAD_NUM];


void *thread_sum_mutex(void *arg_wsk);
void *thread_sum_arr(void *arg_wsk);

double *tab;
double sum = 0; 

int main()
{
    int i; 
    double t1,t2,t3;

    int indecies[THREAD_NUM]; 
    for(i = 0; i < THREAD_NUM; i++) 
        indecies[i] = i;

    tab = (double*) malloc(SIZE * sizeof(double));
    for(i=0; i < SIZE; i++ ) 
        tab[i] = ((double) i + 1) / SIZE; 

    printf("----- Obliczenia sekwencyjne -----\n");
    t1 = czas_zegara();
    for( i = 0; i < SIZE; i++)
        sum += tab[i];

    t1 = czas_zegara() - t1;
    printf("suma = %lf\n", sum);
    printf("Czas = %lf\n", t1);

    printf("----- Watki z mutexem -----\n");
    pthread_mutex_init(&muteks, NULL);
    t1 = czas_zegara();

    sum = 0;
    for(i = 0; i < THREAD_NUM; i++ ) 
        pthread_create(&threads[i], NULL, thread_sum_mutex, (void *) &indecies[i]);
        // code below - synchronization error !!!
        // pthread_create( &threads[i], NULL, thread_sum_mutex, (void *) &i ); 

    for(i = 0; i < THREAD_NUM; i++) 
        pthread_join(threads[i], NULL);

    t1 = czas_zegara() - t1;
    printf("Suma = %lf\n", sum);
    printf("Watki = %d\n", THREAD_NUM);
    printf("Czas: %lf\n", t1);

    // version with array of local sums (to eliminate the mutex) 
    sum = 0;
    printf("----- Watki bez mutexu -----\n");
    t1 = czas_zegara();

    for(i = 0; i < THREAD_NUM; i++) 
    {
        global_array_of_local_sums[i]=0.0;
        pthread_create( &threads[i], NULL, thread_sum_arr, (void *) &indecies[i] );
        // synchronization error !!!
        //pthread_create( &threads[i], NULL, thread_sum_mutex, (void *) &i );
    }

    for(i=0; i<THREAD_NUM; i++ ) 
    {
        pthread_join(threads[i], NULL);
        sum += global_array_of_local_sums[i];
    }

    t1 = czas_zegara() - t1;
    printf("Suma = %lf\n", sum);
    printf("Watki = %d\n", THREAD_NUM);
    printf("Czas: %lf\n", t1);

/*   sum =0; */

/*   printf("Poczatek tworzenia watkow OpenMP\n"); */
/*   t1 = czas_zegara(); */

/* #pragma omp parallel for num_threads(THREAD_NUM) default(none) shared (tab) reduction(+:sum)  */
/*   for(i=0;i<SIZE;i++){ */
/*     sum += tab[i]; */
/*   } */



/*   t1 = czas_zegara() - t1; */
/*   printf("sum = %lf\n", sum); */
/*   printf("Czas obliczen OpenMP = %lf\n", t1); */


 
}

void *thread_sum_mutex(void *arg_wsk)
{

  int i, j, moj_id;
  double moja_sum = 0;

  moj_id = *((int *)arg_wsk); 

  j = ceil((float)SIZE / THREAD_NUM);
  
  // co w przypadku jeśli SIZE jest niepodzielne przez THREAD_NUM i następuje
  // zaokrąglenie w górę? - jak zagwarantować nieprzekroczenie w wersji
  // równoległej górnej granicy iteracji N?
  if(j*THREAD_NUM != SIZE) 
  { 
    printf("Error! Exiting.\n"); 
    exit(0);
  }
  
  for( i = j*moj_id; i < j*(moj_id+1); i++){ 
    moja_sum += tab[i]; 
  }

  pthread_mutex_lock( &muteks );
  sum += moja_sum;
  pthread_mutex_unlock( &muteks );

  pthread_exit( NULL );

}

void *thread_sum_arr(void *arg_wsk)
{
  int i, j, moj_id;
  moj_id = *((int *)arg_wsk ); 

  double tmp = 0.0;
  j = ceil((float) SIZE / THREAD_NUM); // could be double as well 
  for(i = j * moj_id; i < j * (moj_id + 1); i++)
  { 
    // not optimal - possible false sharing of data in single cache line
    //global_array_of_local_sums[moj_id] += tab[i];
    // better - probably would be employed by the compiler anyway
    tmp += tab[i];
  }

  // no critical section, but requires an array with length nr_threads
  global_array_of_local_sums[moj_id] = tmp;

  pthread_exit((void *)0);
}
