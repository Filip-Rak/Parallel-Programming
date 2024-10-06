#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "pomiar_czasu.h"
 
const int liczba = 100000;

main(){

  double a, b, c;
  int i,j,k,l;
  
  inicjuj_czas();

  k = -50000;
  // Start ppmiaru czasu operacji wejscia/wyjscia
  double time_clock = czas_zegara();
  double time_CPU = czas_CPU();
  
  for(i=0;i<liczba;i++)
  {
    printf("%d ",k+i);
  }
  
  // Koniec pomiaru czasu operacji wejscia/wyjscia
  printf("\n");
  printf("Czas wykonania %d operacji wejscia/wyjscia:\n\tCPU: %lf\n\tClock: %lf \n",liczba, czas_CPU() - time_CPU ,czas_zegara() - time_clock);

  a = 1.000001;
  // Start pomiaru czasu operacji arytmetycznej
  time_clock = czas_zegara();
  time_CPU = czas_CPU();
  for(i=0;i<liczba;i++)
  {
    a = 1.000001*a+0.000001; 
  }

  //Koniec pomiaru czasu operacji arytmetycznej
  printf("Wynik operacji arytmetycznych: %lf\n", a);
  printf("Czas wykonania %d operacji wejscia/wyjscia:\n\tCPU: %lf\n\tClock: %lf \n",liczba, czas_CPU() - time_CPU ,czas_zegara() - time_clock);


}
