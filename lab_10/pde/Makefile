CCOMP = gcc
OPT = -fopenmp
LIB = -lm

SOURCE1 = openmp_zaleznosci.c
OUT1 = omp.out

$(OUT1): $(SOURCE1)
	$(CCOMP) $(OPT) $(SOURCE1) -o $(OUT1) $(LIB)

clean:
	rm $(OUT1)