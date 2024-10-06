#define _GNU_SOURCE
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <linux/sched.h>

#include <../pomiar_czasu/pomiar_czasu.h>
#include <time.h>

int zmienna_globalna=0;

#define ROZMIAR_STOSU 1024*64

int funkcja_watku( void* argument )
{

  zmienna_globalna++;

  // Zapisz czas
  struct timespec clock, CPU;
  clock_gettime(CLOCK_REALTIME, &clock);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &CPU);

  int wynik;
  wynik=execv("./program", clock.tv_sec, CPU.tv_sec, clock.tv_nsec, CPU.tv_nsec);
  if(wynik==-1)
    printf("Proces potomny nie wykonal programu\n");
  
  return 0;
}

int main()
{

  void *stos;
  pid_t pid;
  int i; 
  
  stos = malloc( ROZMIAR_STOSU );
  if (stos == 0) {
    printf("Proces nadrzędny - blad alokacji stosu\n");
    exit( 1 );
  }

  // Poczatek mierzenia czasu
  inicjuj_czas();
  double cpu_start = czas_CPU();
  double clock_start = czas_zegara();

  for(i=0;i<10000;i++){

    pid = clone( &funkcja_watku, (void *) stos+ROZMIAR_STOSU, 
		 CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, 0 );

    waitpid(pid, NULL, __WCLONE);

  }

  // Koniec mierzenia czasu
  double cpu_time = czas_CPU() - cpu_start;
  double clock_time = czas_zegara() - clock_start;

  printf("Uzyskany czas po %d iteracji\n", zmienna_globalna);
  printf("CPU: %lf\nZegar: %lf\n", cpu_time, clock_time);

  free( stos );
}
