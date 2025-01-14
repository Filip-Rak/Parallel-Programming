#include<stdlib.h>
#include<stdio.h>
#include<omp.h>

int main()
{
  
#ifdef   _OPENMP
  printf("Kompilator rozpoznaje dyrektywy OpenMP\n");
#endif

    int liczba_watkow;
  
    int a_shared = 1;
    int b_private = 2;
    int c_firstprivate = 3;
    int e_atomic = 5;
    
    // Utworzenie i objecte zmiennej f dyrektywa threadprivate
    static int f_threadprivate;
    #pragma omp threadprivate(f_threadprivate)
  
    printf("przed wejsciem do obszaru rownoleglego -  nr_threads %d, thread ID %d\n", omp_get_num_threads(), omp_get_thread_num());
    printf("\ta_shared \t= %d\n", a_shared);
    printf("\tb_private \t= %d\n", b_private);
    printf("\tc_firstprivate \t= %d\n", c_firstprivate);
    printf("\te_atomic \t= %d\n", e_atomic);

    omp_set_num_threads(5);

    /* Pierszy Blok Rownolegly */  
  
#pragma omp parallel default(none) shared(a_shared, e_atomic) private(b_private) firstprivate(c_firstprivate)
  {
    int d_local_private = a_shared + c_firstprivate;

    // Nadaj zmiennej f numer watku
    f_threadprivate = omp_get_thread_num();

    // Zainicjuj by byla wszedzie taka sama
    b_private = 100;
    
    // Oblicz sumy lokalnie
    for(int i=0;i<10;i++)
        c_firstprivate += omp_get_thread_num();    

    int local_e = 0;
    for(int i=0;i<10;i++)
      local_e += omp_get_thread_num();

    int local_a = 0;
    for(int i=0;i<10;i++)
        local_a += 1; 

    // Specjalne traktowanie dla zmiennej atomic
    // Zaleznosc Write After Write
    // Wiele watkow probowalo zapisac do e_atomic w tym samym czasie
    #pragma omp atomic
    e_atomic += local_e;

    // Przed wypisem zaczekaj az kazdy watek skonczy prace
    // Eliminacja Write After Read
    // a_shared moze byc rozne przy zapisie do d_local_pribae
    #pragma omp barrier

    #pragma omp critical(stdout)
    {
        // Dodaj do zmiennych wspoldzielonych
        // Elimiznacja aleznosci WAW
        // Watki w tym samym czasie nadpisywaly zmienna a_shared
        a_shared += local_a;

        // Wypisz dane
        // Watki mogly jednoczesnie wypisywac dane na konsole co 
        // mogloby prowadzic do nieczytelnych wynikow
        printf("\nw obszarze równoległym: aktualna liczba watkow %d, moj ID %d\n",omp_get_num_threads(), omp_get_thread_num());
        printf("\ta_shared \t= %d\n", a_shared);
        printf("\tb_private \t= %d\n", b_private);
        printf("\tc_firstprivate \t= %d\n", c_firstprivate);
        printf("\td_local_private = %d\n", d_local_private);
        printf("\te_atomic \t= %d\n", e_atomic);
    }
    
    //#pragma omp single
/* #pragma omp master */
/*         { */
    
/*           printf("\ninside single: nr_threads %d, thread ID %d\n", */
/*     	     omp_get_num_threads(), omp_get_thread_num()); */
/*           /\* Get environment information *\/ */
/*           int procs = omp_get_num_procs(); */
/*           int nthreads = omp_get_num_threads(); */
/*           int maxt = omp_get_max_threads(); */
/*           int inpar = omp_in_parallel(); */
/*           int dynamic = omp_get_dynamic(); */
/*           int nested = omp_get_nested(); */
    
/*           /\* Print environment information *\/ */
/*           printf("Number of processors = %d\n", procs); */
/*           printf("Number of threads = %d\n", nthreads); */
/*           printf("Max threads = %d\n", maxt); */
/*           printf("In parallel? = %d\n", inpar); */
/*           printf("Dynamic threads enabled? = %d\n", dynamic); */
/*           printf("Nested parallelism supported? = %d\n", nested); */
    
/*         } */
    
  }  

  /* Poza Pierwszym Blokiem */
  printf("po zakonczeniu obszaru rownoleglego:\n");
  printf("\ta_shared \t= %d\n", a_shared);
  printf("\tb_private \t= %d\n", b_private);
  printf("\tc_firstprivate \t= %d\n", c_firstprivate);
  printf("\te_atomic \t= %d\n", e_atomic);


    /* Drugi Blok Rownolegly */

    // Przejdz tylko po ukonczeniu pierwszego obszaru
    #pragma omp barrier
    printf ("\n/* Drugi Blok Rownolegly */\n");

    #pragma omp parallel default(none)
    {
        // Zapewnienie poprawnego wydruku
        #pragma omp critical(stdout)
        {
            int thread_id = omp_get_thread_num();
            int thread_count = omp_get_num_threads();

            printf("# ID watku: %d\n", thread_id);
            printf("\tLiczba watkow: %d\n", thread_count);
            printf("\tWartosc f: %d\n", f_threadprivate);
        }
    }
  
}
