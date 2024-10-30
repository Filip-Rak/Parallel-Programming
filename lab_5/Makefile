CCOMP = gcc
LOADER = gcc

#OPT = -g -DDEBUG # For debugging
OPT = -O3 -fopenmp -fPIE # For time measurement and position-independent code

INC = -I./include
LIB = -L./lib -lpomiar_czasu -lm -lpthread

# Targets
all: lib/libpomiar_czasu.a calka.out pthreads_suma.out

# Main programs
calka.out: obliczanie_calki.o dekompozycja_petli.o dekompozycja_obszaru.o
	$(LOADER) $(OPT) obliczanie_calki.o dekompozycja_petli.o dekompozycja_obszaru.o -o calka.out $(LIB)

pthreads_suma.out: pthreads_suma.o 
	$(LOADER) $(OPT) pthreads_suma.o -o pthreads_sum.out $(LIB)

# Object files
obliczanie_calki.o: obliczanie_calki.c include/pomiar_czasu.h
	$(CCOMP) -c $(OPT) obliczanie_calki.c $(INC) 

dekompozycja_petli.o: dekompozycja_petli.c
	$(CCOMP) -c $(OPT) dekompozycja_petli.c $(INC) 

dekompozycja_obszaru.o: dekompozycja_obszaru.c
	$(CCOMP) -c $(OPT) dekompozycja_obszaru.c $(INC) 

pthreads_suma.o: pthreads_suma.c include/pomiar_czasu.h
	$(CCOMP) -c $(OPT) pthreads_suma.c $(INC) 

# Library compilation
lib/libpomiar_czasu.a: ../pomiar_czasu/pomiar_czasu.c ../pomiar_czasu/pomiar_czasu.h
	mkdir -p lib
	$(CCOMP) -c -fPIC ../pomiar_czasu/pomiar_czasu.c -o lib/pomiar_czasu.o
	ar rcs lib/libpomiar_czasu.a lib/pomiar_czasu.o

# Cleaning
clean:
	rm -f *.o pthreads_sum.out calka.out lib/*.o lib/libpomiar_czasu.a
