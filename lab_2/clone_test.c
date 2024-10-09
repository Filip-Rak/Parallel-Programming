#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   
#include <sys/types.h> 
#include <sys/wait.h>
#include <sched.h>
#include <linux/sched.h>

#define STACK_SIZE 8192

// Funkcje Pomocnicze
// -------------------
int thread_func4(void *arg)
{
    printf ("CLONE_TEST.C::THREAD_FUNC4 -> Watek wykonal prace\n");
    return 0;
}

int thread_func5_1(void* arg)
{
    const int arr_size = 1900;  // 1900 to limit dla 8192 // 8192 / 1900 = 4.31157894737 
    //const int arr_size = 100;  // Wykorzystanie printf obniza limit
    //const int arr_size = STACK_SIZE / 4.18279295379 -1; // 4.18279295379 -1
    int arr[arr_size];
    for (int i = 0; i < arr_size; i++)
        arr[i] = i;


    printf ("CLONE_TEST.C::THREAD_FUNC5_1 -> Watek wykonal prace\n");
}

void recursive_function(int count) 
{
    // Przydziel tablice na stosie
    // Limit dzialania dla 100 = 8192 * 16
    // Limit dzialania dla 1 = 8192 * 16 (bez zmian)
    int temp[1];
    
    printf("CLONE_TEST.C::RECURSIVE_FUNCTION -> Poziom rekurencji: %d\n", count);
    
    // Zwieksz poziom rekurencji
    recursive_function(count + 1);
}

int thread_func5_2(void* arg)
{
    // Wywolaj funkcje rekurencyjna
    recursive_function(1);
    return 0;
}

int thread_func5_3(void* arg)
{
    // Zmiana katalogu
    if (chdir("test_dir") != 0) 
    {
        perror("CLONE_TEST.C::THREAD_FUNC5_3 -> chdir failed\n");
        return -1;
    }

    return 0;
}

// Funkcje Main
// -------------------

// Zadania
void ex4(void* stack)
{
    // ----------------------------------------------------------
    // Roznica pomiedzy funkcjami fork oraz clone wynika z tego, 
    // ze funkcja clone daje uzytkownikowi wiecej opcji
    // dotyczacow dzielenia zasobow pomiedzy procesem 
    // nadrzednym a potomnym.

    // Obie z tych funkcji tworza nowy proces.
    // Uzycie funkcji clone bez uzycia zadnych flag
    // zezwalajacych na wspoldzielenie zasobow
    // pomiedzy procesami w efekcie daje nam
    // ten sam rezultat co w przypadku funkcji fork.
    // ----------------------------------------------------------

    // Wywolanie metody clone bez flag wspoldzielenia pamieci
    // Jedyna flaga jest SIGCHLD, ktora mowi, ze proces macierzysty
    // dostanie sygnal, gdy prace zakonczy proces potomny
    pid_t pid = clone(thread_func4, stack + STACK_SIZE, SIGCHLD, NULL);

    if (pid == -1)
    {
        perror("CLONE_TEST.C::EX4 -> Watek nie utworzony\n");
        return;
    }

    waitpid(pid, NULL, 0);
    
}

void ex5_1(void* stack)
{
    pid_t pid = clone(thread_func5_1, stack + STACK_SIZE, CLONE_VM, NULL);

    if (pid == -1)
    {
        perror("CLONE_TEST.C::EX5_1 -> Watek nie utworzony\n");
        return;
    }

    waitpid(pid, NULL, 0);

    // W jaki sposób można określić rozmiar stosu dla nowo tworzonego wątku? 
    // -> Jest to rozmiar stosu przekazywanego jako argument
    // W jaki sposób można przekroczyć ten rozmiar? (jednocześnie testując jego wielkość)
    // -> Rekurencja, Zmienne lokalne
    // Ile wynosi domyślny rozmiar stosu?
    // -> ulimit -s: 8192
}

void ex5_2(void* stack)
{
    pid_t pid = clone(thread_func5_2, stack + STACK_SIZE, CLONE_VM, NULL);

    if (pid == -1)
    {
        perror("CLONE_TEST.C::EX5_2 -> Watek nie utworzony\n");
        return;
    }

    waitpid(pid, NULL, 0);
}

void ex5_3(void* stack)
{
    // --------------- Test flagi CLONE_FS ---------------
    // W katalogu /tmp zostal umieszczony plik testfile.
    // Obecnym katalog roboczy nie zawiera pliku o takiej nazwie.
    // Wywolywany watek z uzyciem polecenia chdir, zmieni katalog roboczy
    // na ten w ktorym znajduje sie plik testowy
    // Po zakonczeniu pracy watku, proces nadrzedny (funckaj ex5_3) sprobuje otworzyc plik.
    // Ponizej znajduja sie dwa wywolania funkcji clone, jedna z nich zawiera flage CLONE_FS
    // 
    // Brak flagi CLONE_FS powinien sprawi, ze katalog roboczy nie zostanie 
    // zmieniony przez watek - przez co plik nie zostanie otwarty
    // Obecnosc tej flagi pozwala na otwarcie pliku
    // ---------------------------------------------------

    // Watek z flaga CLONE_FS
    // pid_t pid = clone(thread_func5_3, stack + STACK_SIZE, SIGCHLD | CLONE_FS, NULL);

    // Watek bez flagi CLONE_FS
    pid_t pid = clone(thread_func5_3, stack + STACK_SIZE, SIGCHLD, NULL);

    if (pid == -1)
    {
        perror("CLONE_TEST.C::EX5_3 -> Watek nie utworzony\n");
        return;
    }

    waitpid(pid, NULL, 0);

    // Proba otwarcia pliku w nowym katalogu
    FILE* fd = fopen("testfile", "r");
    if (!fd) 
    {
        perror("CLONE_TEST.C::EX5_3 ->");
        return;
    }

    printf("CLONE_TEST.C::EX5_3 -> Plik zostal otworzony\n");
    fclose(fd);
}

int main()
{
    // Alokacja pamieci stosu
    void* stack = malloc(STACK_SIZE);
    if (!stack)
    {
        perror("CLONE_TEST.C::MAIN -> Alokacja pamieci stosu sie nie powiodla\n");
        return -1;
    }

    // Wykonanie zadan
    // ex4(stack);
    // ex5_1(stack);
    // ex5_2(stack);
    ex5_3(stack);

    // Zwalnianie zaalokowanej pamieci
    free(stack);
}