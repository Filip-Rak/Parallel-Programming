#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

#define WYMIAR 10
#define epsilon 1e-6

void check_result_validity(double suma, double suma_watki, bool* check)
{
	if (fabs(suma - suma_watki) < epsilon)
		printf("Wynik zgodny\n");
	else 
	{
		printf("Wynik niezgodny\n");
		*check = false;
	}
}

int main ()
{
  	double arr[WYMIAR][WYMIAR];

	// Wypelnienie tablicy
  	for(int i=0;i<WYMIAR;i++) 
		for(int j=0;j<WYMIAR;j++) 
			arr[i][j]=1.02*i+1.01*j;

  	// podwójna pętla - sekwencyjnie
  	double suma=0.0;
  	for(int i=0;i<WYMIAR;i++) 
	{
    	for(int j=0;j<WYMIAR;j++) 
		{
    	  suma += arr[i][j];
    	}
  	}
  
  	printf("Suma wyrazów tablicy: %lf\n", suma);
	bool result_correct = true;


	/* Dekompozycja Wierszowa */
	printf("\n/* Dekompozycja Wierszowa */\n");

	double suma_watki = 0.f;
	#pragma omp parallel for default(none) schedule(static, 2) shared(arr) reduction(+:suma_watki) ordered
	for (int i = 0; i < WYMIAR; i++)
	{
		#pragma omp ordered
		{
			for (int j = 0; j < WYMIAR; j++)
			{
				suma_watki += arr[i][j];
				printf("arr[%d][%d]=W%d\n", i, j, omp_get_thread_num());
			}
		}
		printf("\n");
	}

	// Sprawdzenie poprawności
	printf("Suma dekompozycja wierszowa: %lf\n", suma_watki);
	check_result_validity(suma, suma_watki, &result_correct);

	/* Dekompozycja Kolumnowa Wewnętrzna */
	printf("\n/* Dekompozycja Kolumnowa Wewnętrzna */\n");
	suma_watki = 0.f;
	for (int i = 0; i < WYMIAR; i++)
	{
		#pragma omp parallel for default(none) schedule(dynamic) shared(arr, i) reduction(+:suma_watki) ordered
		for (int j = 0; j < WYMIAR; j++)
		{
			suma_watki += arr[i][j];
			#pragma omp ordered
			{
				printf("arr[%d][%d]=W%d\n", i, j, omp_get_thread_num());
			}
		}
		printf("\n");
	}

	// Sprawdzenie poprawności
 	printf("Suma dekompozycja kolumnowa wewn.: %lf\n", suma_watki);
	check_result_validity(suma, suma_watki, &result_correct);

	/* Dekompozycja Kolumnowa Zewnetrzna */
	printf("\n/* Dekompozycja Kolumnowa Zewnetrzna */\n");

	suma_watki = 0.f;
	#pragma omp parallel for default(none) schedule(static, 2) shared(arr, suma_watki) ordered
	for (int i = 0; i < WYMIAR; i++)
	{
		double local_sum = 0.f;
		for (int j = 0; j < WYMIAR; j++)
		{
			local_sum += arr[j][i];

			#pragma omp ordered
			printf("arr[%d][%d]=W%d\n", j, i, omp_get_thread_num());
		}
		printf("\n");

		#pragma omp critical
		suma_watki += local_sum;
	}

	// Sprawdzenie poprawności
	printf("Suma dekompozycja kolumnowa zewn.: %lf\n", suma_watki);
	check_result_validity(suma, suma_watki, &result_correct);

	/* Dekompozycja 2D */
	printf("\n/* Dekompozycja 2D */\n");

	// Zezwol na zrownoleglenie petli wewnetrznych
	omp_set_nested(true);

	suma_watki = 0.f;
	#pragma omp parallel for default(none) schedule(static, 2) num_threads(3) shared(arr, suma_watki) ordered
	for (int i = 0; i < WYMIAR; i++)
	{
		int outer_id = omp_get_thread_num();

		#pragma omp parallel for default(none) schedule(static, 2) num_threads(3) shared(arr, suma_watki, i, outer_id) ordered
		for (int j = 0; j < WYMIAR; j++)
		{
			int inner_id = omp_get_thread_num();

			#pragma omp critical
			suma_watki += arr[i][j];

			#pragma omp ordered
			printf("arr[%d][%d]=W[%d][%d]\n", i, j, outer_id, inner_id);
		}

		#pragma omp ordered
		printf("\n");
	}

	// Sprawdzenie poprawności
	printf("Suma dekompozycja 2D.: %lf\n", suma_watki);
	check_result_validity(suma, suma_watki, &result_correct);

	/* Dekompozycja Wierszowa Z Tablicą 1D */
	printf("\n/* Dekompozycja Wierszowa Z Tablicą 1D */\n");
	
	// Zaalokuj pamiec na tablice
	int thread_num = omp_get_max_threads();
	double* result_arr = malloc(sizeof(double) * thread_num);
	for (int i = 0; i < thread_num; i++)
		result_arr[i] = 0;

	// Oblicz czesci w kazdym watku lokalnie
	suma_watki = 0.0f;
	#pragma omp parallel for default(none) shared(arr, result_arr) ordered
	for (int i = 0; i < WYMIAR; i++)
	{
		int thread_id = omp_get_thread_num();
		for (int j = 0; j < WYMIAR; j++)
		{
			result_arr[thread_id] += arr[i][j];

			#pragma omp ordered
			printf("arr[%d][%d]=W%d\n", i, j, thread_id);
		}

		#pragma omp ordered
		printf("\n");
	}

	// Zsumuj lokalne wyniki wszystkich watkow
	for(int i = 0; i < thread_num; i++)
		suma_watki += result_arr[i];

	// Zwolnij pamiec dynamiczna
	free(result_arr);

	// Sprawdzenie poprawności
	printf("Suma dekompozycja wierszowej z tablica pomocnicza: %lf\n", suma_watki);
	check_result_validity(suma, suma_watki, &result_correct);
	
	/* Sprawdz ogolna popranosc wynikow */
	if(result_correct)
		printf("Wszystkie wyniki sa zgodne\n");
	else 
		printf("Nie wszystkie wyniki sa zgodne\n");
}
