linker = gcc
options = -fopenmp

source_file = openmp_petle_simple.c
result_file = loop.out

$(result_file): $(source_file)
	$(linker) $(options) $(source_file) -o $(result_file)

clean:
	rm $(result_file)