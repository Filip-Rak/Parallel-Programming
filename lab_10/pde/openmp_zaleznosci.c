#include<stdlib.h>
#include<stdio.h>
#include<omp.h>
#include<math.h>

#define N 1000000

int main()
{

    /* Alokacja i Inicjalizacja */
    double* A = malloc((N+2)*sizeof(double));
    double* B = malloc((N+2)*sizeof(double));

    for(int i=0;i<N+2;i++) 
        A[i] = (double)i/N;

    for(int i=0;i<N+2;i++) 
        B[i] = 1.0 - (double)i/N;

    double t1 = omp_get_wtime();
    for(int i=0; i<N; i++)
        A[i] += A[i + 2] + sin(B[i]);

    t1 = omp_get_wtime() - t1;

    double suma; = 0.0;
    for(int i=0;i<N+2;i++) 
        suma+=A[i];
    
    printf("suma %lf, czas obliczen %lf\n", suma, t1);

    /* Inicjalizacja danych */
    for(int i=0;i<N+2;i++) 
        A[i] = (double)i/N;
    for(int i=0;i<N+2;i++) 
        B[i] = 1.0 - (double)i/N;

    t1 = omp_get_wtime();

    /* Wersja Rownolegla */

    t1 = omp_get_wtime() - t1;

    suma = 0.0;
    for(i=0;i<N+2;i++) suma+=A[i];
    printf("suma %lf, czas obliczen rownoleglych %lf\n", suma, t1);


}
