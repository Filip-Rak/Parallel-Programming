#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>

typedef struct
{
    int threads;
    int counter;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
} Barrier;

extern void bariera_init(Barrier*, int);
extern void barrier_destroy(Barrier* barrier);
extern void bariera(Barrier*);

Barrier barrier1;
Barrier barrier2;

#define LICZBA_W 4  // Only even numbers
// #define LICZBA_W 44

pthread_t watki[LICZBA_W];

void *thread_func1( void *arg){

  int i, moj_id;

  moj_id = *( (int *) arg ); 

  printf("przed bariera 1.1 - watek %d\n",moj_id);
  bariera(&barrier1);

  printf("przed bariera 1.2 - watek %d\n",moj_id);
  bariera(&barrier1);

  printf("przed bariera 1.3 - watek %d\n",moj_id);
  bariera(&barrier1);

  printf("przed bariera 1.4 - watek %d\n",moj_id);
  bariera(&barrier1);

  printf("po barierze 1. - watek %d\n",moj_id);  

  pthread_exit( (void *)0);
}

void *thread_func2( void *arg)
{

  int i, moj_id;

  moj_id = *( (int *) arg ); 

  printf("przed bariera 2.1 - watek %d\n",moj_id);
  bariera(&barrier2);

  printf("przed bariera 2.2 - watek %d\n",moj_id);
  bariera(&barrier2);

  printf("przed bariera 2.3 - watek %d\n",moj_id);
  bariera(&barrier2);

  printf("przed bariera 2.4 - watek %d\n",moj_id);
  bariera(&barrier2);

  printf("po barierze 2. - watek %d\n",moj_id);  

  pthread_exit( (void *)0);
}

int main( int argc, char *argv[] )
{

  int i, indeksy[LICZBA_W]; for(i=0;i<LICZBA_W;i++) indeksy[i]=i; 

  bariera_init(&barrier1, LICZBA_W / 2);
  bariera_init(&barrier2, LICZBA_W / 2);

  for(i=0; i<LICZBA_W; i++ ) 
  {
    if (i % 2 == 0)
      pthread_create(&watki[i], NULL, thread_func1, (void *) &indeksy[i]);
    else 
      pthread_create(&watki[i], NULL, thread_func2, (void *) &indeksy[i]);
  }

  for(i=0; i<LICZBA_W; i++ ) 
    pthread_join( watki[i], NULL );


  // CLean dynamic memory
  barrier_destroy(&barrier1);
  barrier_destroy(&barrier2);
  pthread_exit( NULL);  
}
