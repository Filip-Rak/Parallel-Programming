# Kompilator i loader
CCOMP = gcc
LOADER = gcc

# Flagi optymalizacji
OPT = -O3

# Flagi kompilatora i linkera
CFLAGS = $(OPT) -I../pomiar_czasu
LDFLAGS = -L../pomiar_czasu
LDLIBS = -lpomiar_czasu -lm

# Pliki obiektowe
OBJ = moj_program.o pomiar_czasu.o

# Glowna regula: jak utworzyc program
moj_program: $(OBJ)
	$(LOADER) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

# Automatyczna regula dla plikow .o
%.o: %.c
	$(CCOMP) $(CFLAGS) -c $< -o $@

# Plik pomiar_czasu.o zalezy od zrodla pomiar_czasu.c w innym folderze
pomiar_czasu.o: ../pomiar_czasu/pomiar_czasu.c
	$(CCOMP) $(CFLAGS) -c $< -o $@

# Regula clean, ktora usuwa pliki .o oraz program
clean:
	rm -f $(OBJ) moj_program
