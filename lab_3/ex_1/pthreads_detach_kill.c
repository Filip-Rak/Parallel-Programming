#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include <unistd.h>

int zmienna_wspolna=0;

#define WYMIAR 1000
#define ROZMIAR WYMIAR*WYMIAR
double a[ROZMIAR],b[ROZMIAR],c[ROZMIAR];


double czasozajmowacz(){
  int i, j, k;
  int n=WYMIAR;
  for(i=0;i<ROZMIAR;i++) a[i]=1.0*i;
  for(i=0;i<ROZMIAR;i++) b[i]=1.0*(ROZMIAR-i);
  for(i=0;i<n;i++){
    for(j=0;j<n;j++){
      c[i+n*j]=0.0;
      for(k=0;k<n;k++){
	c[i+n*j] += a[i+n*k]*b[k+n*j];
      }
    }
  }
  return(c[ROZMIAR-1]);
}

void * zadanie_watku (void * arg_wsk)
{
	int nr_watku = *(int*)arg_wsk;

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	printf("\twatek potomny nr: %d: uniemozliwione zabicie\n", nr_watku);

	czasozajmowacz();

	printf("\twatek potomny nr: %d: umozliwienie zabicia\n", nr_watku);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	pthread_testcancel();

	zmienna_wspolna++;
	printf("\twatek potomny nr: %d: zmiana wartosci zmiennej wspolnej\n", nr_watku);

	return (void*) 42;
}

int main()
{
	pthread_t tid;
	pthread_attr_t attr;
	void *wynik;
	int i;

	// ----------------THREAD 1-----------------

	//Wątek przyłączalny	
	printf("watek glowny: tworzenie watku potomnego nr 1\n");

	// Jest dosyc duzy odstep czasowy pomiedzy twoerzeniem watkow
	// wiec trzymanie ich numerow w pojedynczym miejscu w pamieci
	// powinno byc ok
	int thread_number = 1;

	/*Tu wstaw kod tworzenia wątku z domyślnymi własnościami*/
	// i = return_code, tid = id_watku, NULL1 = atrubuty
	// zadanie_watku = funkcja, NULL2 = argumenty
	i = pthread_create(&tid, NULL, zadanie_watku, &thread_number);
	if (i != 0)
	{
		printf("Proba utworzneia watku nie powiodla sie\n");
		printf("Kod bledu %d", i);
	}

	// czas na uruchomienie watku
	sleep(2); 

	printf("\twatek glowny: wyslanie sygnalu zabicia watku\n");
	if(pthread_cancel(tid) != 0)
		printf("\tCANCEL FAILED\n");

	//Co nalezy zrobić przed sprawdzeniem czy wątki się skonczyły?
	if(pthread_join(tid, &wynik) != 0)
		printf("\tJOIN FAILED\n");

	if (wynik == PTHREAD_CANCELED)
		printf("\twatek glowny: watek potomny zostal zabity\n");
	else
		printf("\twatek glowny: watek potomny NIE zostal zabity - zakonczyl sie z: %ld\n", (long)wynik);
		

	//Odłączanie wątku
	// Czy jest to konieczne po pthread_join()?
	pthread_detach(tid);
	zmienna_wspolna = 0;
	wynik = NULL;

	// ----------------THREAD 2-----------------

	printf("watek glowny: tworzenie watku potomnego nr 2\n");
	/*Tu wstaw kod tworzenia wątku z domyślnymi własnościami*/
	thread_number = 2;
	i = pthread_create(&tid, NULL, zadanie_watku, &thread_number);
	if (i != 0)
	{
		printf("Proba utworzneia watku nie powiodla sie\n");
		printf("Kod bledu %d", i);
	}

	sleep(2); // czas na uruchomienie watku

	printf("\twatek glowny: odlaczenie watku potomnego\n");
	//Instrukcja odłączenia?
	pthread_detach(tid);

	printf("\twatek glowny: proba wyslania sygnalu zabicia watku odlaczonego\n");
	if(pthread_cancel(tid) == 0)
		printf("\t\tSygnal zostal wyslany i watek go otrzymal lub juz zakonczyl prace\n");
	else 
		printf("\t\tWyslanie prosby o zakonczenie pracy watku nie powiodlo sie\n");

	//Czy watek został zabity? Jak to sprawdzić?
	// W przypadku watku ktory zostal odlaczony
	// nie możemy sprawdzic czy faktycznie on zostanie "zabity"
	// Mozemy sprawdzic czy sygnal anulowania zostal wyslany
	// Funkcja pthread_cancel wtedy zwroci 0
	// Wyslanie sygnalu jednak nie gwarantuje nam, ze watek zostanie anulowany
	// Anulowanie zalezy juz od tego jaka zostala napisana funkcja watku

	// ----------------THREAD 3-----------------

	//Wątek odłączony

	//Inicjacja atrybutów?
	// Iicjalizacja wartosciami domyslnymi
	pthread_attr_init(&attr);
	//Ustawianie typu watku na odłaczony
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	printf("watek glowny: tworzenie odlaczonego watku potomnego nr 3\n");
	thread_number = 3;
	pthread_create(&tid, &attr, zadanie_watku, &thread_number);

	//Niszczenie atrybutów
	pthread_attr_destroy(&attr);

	printf("\twatek glowny: koniec pracy, watek odlaczony pracuje dalej\n");
	pthread_exit(NULL); // co stanie sie gdy uzyjemy exit(0)?
	// exit(0);

	// Funkcja void pthread_exit (void* retval) 
	// zakancza watek w ktorym zostanie wywolana.
	// Funkcja ta jednak nie przerwie pracy innym, ktore
	// zostaly wywolane przez watek uzywajacy tej funkcji.
	// Argument zostanie przekazany watkowi nadrzednemu
	// Moze byc odebrany przez funkcje pthread_join(id, &wynik)
	// jako wynik

	// Q: co stanie sie gdy uzyjemy exit(0)?
	// A: Program zakonczy sie przedwczesnie,
	// eliminujac wszystkie watki
}


