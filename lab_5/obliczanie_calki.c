#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "pomiar_czasu.h"

#ifndef M_PI // standardy C99 i C11 nie wymagają definiowania stałej M_PI
#define M_PI (3.14159265358979323846)
#endif

// double funkcja (double x );
double funkcja (double x ){ return( sin(x) ); }
double calka_sekw(double a, double b, double dx);
double calka_zrownoleglenie_petli(double a, double b, double dx, int l_w, bool cyclic);
double calka_dekompozycja_obszaru(double a, double b, double dx, int l_w);

int main( int argc, char *argv[] )
{

  int i; 
  double t1,t2,t3;
  double a, b, dx, calka;

  printf("\nProgram obliczania całki z funkcji (sinus) metodą trapezów.\n");

  a = 0.0;
  //printf("\nPodaj lewy kraniec przedziału całkowania: "); scanf("%lf", &a);

  b = M_PI;
  //printf("\nPodaj prawy kraniec przedziału całkowania: "); scanf("%lf", &b);

  dx = pow(10, -8);
  // printf("\nPodaj wysokość pojedynczego trapezu:  "); scanf("%lf", &dx);

  int l_w=0;
  printf("Podaj liczbę wątków:  "); scanf("%d", &l_w);

  t1 = czas_zegara();
  calka = calka_sekw(a, b, dx);
  t1 = czas_zegara() - t1;
  printf("\nObliczenia sekwencyjne:\n");
  printf("\tCzas wykonania %lf. \n\tObliczona całka = %.15lf\n", t1, calka);

  t1 = czas_zegara();
  calka = calka_zrownoleglenie_petli(a, b, dx, l_w, true);

  t1 = czas_zegara() - t1;
  printf("Zrownoleglenie petli (cykliczne):\n");
  printf("\tCzas wykonania %lf. \n\tObliczona całka = %.15lf\n", t1, calka);

  t1 = czas_zegara();
  calka = calka_zrownoleglenie_petli(a, b, dx, l_w, false);

  t1 = czas_zegara() - t1;
  printf("Zrownoleglenie petli (blokowe):\n");
  printf("\tCzas wykonania %lf. \n\tObliczona całka = %.15lf\n", t1, calka);

  t1 = czas_zegara();
  calka = calka_dekompozycja_obszaru(a, b, dx, l_w);

  t1 = czas_zegara() - t1;
  printf("Dekompozycja obszaru:\n");
  printf("\tCzas wykonania %lf. \n\tObliczona całka = %.15lf\n", t1, calka);

}

double calka_sekw(double a, double b, double dx)
{
  int N = ceil((b-a)/dx);
  double dx_adjust = (b-a)/N;
  double integral = 0.0;


  double multiplier = 0.5 * dx_adjust;
  double previous_value = funkcja(a);

  for(int i = 0; i < N; i++)
  {
    double x1 = a + (i + 1) * dx_adjust;
    double current_value = funkcja(x1);

    integral += multiplier * (previous_value + current_value);

    previous_value = current_value;
  }

  return integral;
}
