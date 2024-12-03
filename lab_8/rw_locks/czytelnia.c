#include"czytelnia.h"


/*** Implementacja procedur interfejsu ***/

int my_read_lock_lock(cz_t* cz_p) 
{
    // Staramy sie tylko 'namieszac' czytelnikom by dac
    // pisarzom szanse na wejscie
    // ze wzgledu na to, ze pozostale meahcnizmy chronia nadal wejscie
    // szynchronizacja, tj. jej brak, nie powinnna byc problemem
    // while(cz_p->closed_for_readers) { usleep(100); }

    pthread_rwlock_rdlock(&cz_p->rwlock); // Zamek odczytu
    cz_p->l_c++; // Licznik czytelnikow
    check_for_error(cz_p);
}

int my_read_lock_unlock(cz_t* cz_p) 
{
    cz_p->l_c--; // Licznik czytelnikow
    check_for_error(cz_p);
    cz_p->closed_for_readers = true;
    pthread_rwlock_unlock(&cz_p->rwlock); // Zwolnienie zamka

    // Niech czytelnik sobie odpocznie przed powrotem do biblioteki
    // To da szanse pisarzom
    usleep(2000000);
}

int my_write_lock_lock(cz_t* cz_p) 
{
    pthread_rwlock_wrlock(&cz_p->rwlock); // Zamek zapisu
    cz_p->l_p++; // Licznik pisarzy
    check_for_error(cz_p);
}

int my_write_lock_unlock(cz_t* cz_p) 
{
    cz_p->l_p--; // Licznik pisarzy
    check_for_error(cz_p);
    cz_p->closed_for_readers = false;
    pthread_rwlock_unlock(&cz_p->rwlock); // Zwolnienie zamka
}

void inicjuj(cz_t* cz_p)
{
  cz_p->l_p = 0;
  cz_p->l_c = 0;  
  cz_p->closed_for_readers = false;
  pthread_rwlock_init(&cz_p->rwlock, NULL);
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

    bool failure = false;
    if (writer_count > 1) 
    {
        printf("### Blad: Jednoczesna obecność więcej niż jednego pisarza.\n");
        failure = true;
    } 
    if (writer_count == 1 && reader_count > 0) 
    {
        printf("### Blad: Jednoczesna obecność pisarza i czytelników.\n");
        failure = true;
    } 
    if (writer_count < 0) 
    {
        printf("### Blad: Liczba pisarzy jest ujemna.\n");
        failure = true;
    } 
    else if (reader_count < 0) 
    {
        printf("### Blad: Liczba czytelników jest ujemna.\n");
        failure = true;
    }  

    if (failure)
    {
        pthread_rwlock_destroy(&obj->rwlock);
        exit(0);
    }

    #endif
}