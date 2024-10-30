CCOMP = gcc
LOADER = gcc

#OPT = -g -DDEBUG # For debugging
OPT = -O3 -fopenmp # For time measurement

INC = -I./include
LIB = -L../pomiar_czasu -lpomiar_czasu -lm -lpthread
# LIB = -L./lib -lpomiar_czasu -lm -lpthread

all: calka.out pthreads_suma.out

calka.out: obliczanie_calki.o dekompozycja_petli.o dekompozycja_obszaru.o
	$(LOADER) $(OPT) obliczanie_calki.o dekompozycja_petli.o dekompozycja_obszaru.o -o calka.out $(LIB)

obliczanie_calki.o: obliczanie_calki.c include/pomiar_czasu.h
	$(CCOMP) -c $(OPT) obliczanie_calki.c $(INC) 

dekompozycja_petli.o: dekompozycja_petli.c
	$(CCOMP) -c $(OPT) dekompozycja_petli.c $(INC) 

dekompozycja_obszaru.o: dekompozycja_obszaru.c
	$(CCOMP) -c $(OPT) dekompozycja_obszaru.c $(INC) 

pthreads_suma.out: pthreads_suma.o 
	$(LOADER) $(OPT) pthreads_suma.o -o pthreads_sum.out $(LIB)

pthreads_suma.o: pthreads_suma.c include/pomiar_czasu.h
	$(CCOMP) -c $(OPT) pthreads_suma.c $(INC) 

clean:
	rm -f *.o pthreads_sum.out calka.out
