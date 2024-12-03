#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

#include"czytelnia.h"


/*** Implementacja procedur interfejsu ***/

int my_read_lock_lock(cz_t* cz_p) 
{
    pthread_mutex_lock(&cz_p->mutex);

    // Czekanie, az nie bedzie pisarzy
    while (cz_p->l_p > 0) 
    {
        pthread_cond_wait(&cz_p->readers, &cz_p->mutex);
    }
    
    cz_p->l_c++; // Czytelnik wchodzi
    check_for_error(cz_p);

    pthread_mutex_unlock(&cz_p->mutex);
}


int my_read_lock_unlock(cz_t* cz_p) 
{
    pthread_mutex_lock(&cz_p->mutex);

    cz_p->l_c--; // Czytelnik wychodzi
    check_for_error(cz_p);

    if (cz_p->l_c == 0) 
    {
        pthread_cond_signal(&cz_p->writers); // Budzimy pisarzy, jesli nie ma juz czytelnikow
    }

    pthread_mutex_unlock(&cz_p->mutex);
}


int my_write_lock_lock(cz_t* cz_p) 
{
    pthread_mutex_lock(&cz_p->mutex);

    // Czekanie, az nie bedzie czytelnikow ani pisarzy
    while (cz_p->l_p > 0 || cz_p->l_c > 0) 
    {
        pthread_cond_wait(&cz_p->writers, &cz_p->mutex);
    }

    cz_p->l_p++; // Pisarz wchodzi
    check_for_error(cz_p);

    pthread_mutex_unlock(&cz_p->mutex);
}


int my_write_lock_unlock(cz_t* cz_p) 
{
    pthread_mutex_lock(&cz_p->mutex);

    cz_p->l_p--; // Pisarz wychodzi
    check_for_error(cz_p);

    // Budzenie kolejnych watkow
    if (cz_p->l_p == 0) 
    {
        pthread_cond_broadcast(&cz_p->readers); // Budzimy wszystkich czytelnikow
        pthread_cond_signal(&cz_p->writers);   // Budzimy pisarzy
    }

    pthread_mutex_unlock(&cz_p->mutex);
}

void inicjuj(cz_t* cz_p)
{
 
    cz_p->l_p = 0;
    cz_p->l_c = 0;  
    pthread_mutex_init(&cz_p->mutex, NULL);
    pthread_cond_init(&cz_p->readers, NULL);
    pthread_cond_init(&cz_p->writers, NULL);
}

void czytam(cz_t* cz_p)
{
    check_for_error(cz_p);
    usleep(rand()%3000000);
}

void pisze(cz_t* cz_p)
{
    check_for_error(cz_p);
    usleep(rand()%3000000);
}

void check_for_error(cz_t* obj)
{
    #ifdef ERROR_DEBUG

    int writer_count = obj -> l_p;
    int reader_count = obj -> l_c;

    printf("Pisarze: %d\nCzytelnicy: %d\n", writer_count, reader_count); 
    if (writer_count > 1) 
    {
        printf("### Blad: Jednoczesna obecność więcej niż jednego pisarza.\n");
        exit(0);
    } 
    else if (writer_count == 1 && reader_count > 0) 
    {
        printf("### Blad: Jednoczesna obecność pisarza i czytelników.\n");
        exit(0);
    } 
    else if (writer_count < 0) 
    {
        printf("### Blad: Liczba pisarzy jest ujemna.\n");
        exit(0);
    } 
    else if (reader_count < 0) 
    {
        printf("### Blad: Liczba czytelników jest ujemna.\n");
        exit(0);
    }  

    #endif
}