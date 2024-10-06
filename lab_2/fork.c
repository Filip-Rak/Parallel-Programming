#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "pomiar_czasu.h"

int zmienna_globalna=0;

int main(){
  
  int pid, wynik, i;

  inicjuj_czas();
  double cpu_start = czas_CPU();
  double clock_start = czas_zegara();
  
  for(i=0;i<5;i++)
  {
    
    pid = fork(); 			
    
    if(pid==0)
    { 			
      zmienna_globalna++;
      char* arg[] = {"./program2", "Spongebob", "Squarepants", NULL};
      wynik=execv(arg[0], arg);
      if(wynik==-1)
        printf("Proces potomny nie wykonal programu\n");

      exit(0);
      
    } 
    else 
    {					
      
      wait(NULL);
      
    }
    
  }

  //drukuj_czas();
  double cpu_end = czas_CPU();
  double clock_end = czas_zegara();

  printf("Ilosc iteracji: %d\n", i);
  printf("CPU:\t%lf\n", cpu_end - cpu_start);
  printf("Zegar:\t%lf\n", clock_end - clock_start);
  
}

  
