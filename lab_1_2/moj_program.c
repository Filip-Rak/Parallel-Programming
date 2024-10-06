#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "../pomiar_czasu/pomiar_czasu.h"
 
const int liczba = 100000;

int main()
{
  double a, b, c;
  int i,j,k,l;

  // -- Operacja Wejscia/Wyjscia --
  k = -50000;

  // Poczatek mierzenia
  inicjuj_czas();

  for(i=0;i<liczba;i++)
    printf("%d ",k+i);
    
  printf("\n--------------------\n");
  printf("Czas wykonania %d operacji wejscia/wyjscia: \n",liczba);

  // Koniec mierzenia
  drukuj_czas();

  // -- Operacja Arytmetyczna
  a = 1.000001;

  // Poczatek mierzenia
  double t1 = czas_CPU();
  double t2 = czas_zegara();

  for(i=0;i<liczba;i++)
    a = 1.000001*a+0.000001; 

  printf("\n--------------------\n");
  printf("Wynik operacji arytmetycznych: %lf\n", a);
  printf("Czas wykonania %d operacji arytmetycznych: \n",liczba);
  printf("Czas CPU: %lf\nCzas zegara: %lf\n", czas_CPU() - t1, czas_zegara() - t2);
}
