#ifndef _czytelnia_
#define _czytelnia_

#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdbool.h>

/*** Definicje typow zmiennych ***/
typedef struct 
{
  int l_p; // liczba piszacych
  int l_c; // liczba czytajacych
  pthread_mutex_t mutex;
  pthread_cond_t readers;
  pthread_cond_t writers;
  bool closed_for_readers;
} cz_t;

/*** Deklaracje procedur interfejsu ***/
void inicjuj(cz_t* czytelnia_p);
void czytam(cz_t* czytelnia_p);
void pisze(cz_t* czytelnia_p);

int my_read_lock_lock(cz_t* czytelnia_p);
int my_read_lock_unlock(cz_t* czytelnia_p);
int my_write_lock_lock(cz_t* czytelnia_p);
int my_write_lock_unlock(cz_t* czytelnia_p);

void check_for_error(cz_t* obj);

#endif
