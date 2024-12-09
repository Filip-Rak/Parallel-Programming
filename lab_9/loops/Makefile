linker = gcc
options = -fopenmp

source1 = openmp_petle_simple.c
source2 = openmp_petle.c
result1 = loop1.out
result2 = loop2.out

all: $(result1) $(result2)

$(result2): $(source2)
	$(linker) $(options) $(source2) -o $(result2)

$(result1): $(source1)
	$(linker) $(options) $(source1) -o $(result1)

clean:
	rm $(result1)