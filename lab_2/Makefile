# kompilator c
CCOMP = gcc

# konsolidator
LOADER = gcc

# opcje optymalizacji:
# wersja do debugowania
# OPT = -g -DDEBUG -O0
# wersja zoptymalizowana do mierzenia czasu
OPT = -O3

# pliki naglowkowe
INC = -I../pomiar_czasu

# biblioteki
LIB = -L../pomiar_czasu -lpomiar_czasu -lm

all: fork clone program program2 threads clone_test

fork: fork.o 
	$(LOADER) $(OPT) fork.o -o fork $(LIB)

fork.o: fork.c ../pomiar_czasu/pomiar_czasu.h
	$(CCOMP) -c $(OPT) fork.c $(INC) 

clone: clone.o 
	$(LOADER) $(OPT) clone.o -o clone $(LIB)

program: program.c
		$(LOADER) $(OPT) program.c -o program

program2: program2.c
		$(LOADER) $(OPT) program2.c -o program2

threads: threads.c
		$(LOADER) $(OPT) threads.c -o threads
		
clone_test: clone_test.c
		$(LOADER) $(OPT) clone_test.c -o clone_test

clone.o: clone.c ../pomiar_czasu/pomiar_czasu.h
	$(CCOMP) -c $(OPT) clone.c $(INC) 

clean:
	rm -f *.o fork clone program program2 threads clone_test
