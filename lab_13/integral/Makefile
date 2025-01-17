# Compiller
LOADER = gcc

# Options
OPT = -O3 -fopenmp

# Files
SOURCE1 = calka_omp.c
OUT1 = integral.out

# Commands
all: $(OUT1) run 

run: $(OUT1)
	./$(OUT1)

$(OUT1): $(SOURCE1)
	$(LOADER) $(OPT) $(SOURCE1) -o $(OUT1)
	
clean:
	rm $(OUT1)
