# Cpmpiller
CCOMP = mpicc
LOADER = mpicc
MPI = mpiexec -oversubscribe

# Options
OPT = -g -DDEBUG
LIB = -lm
PROC = 4

# Files
SOURCE1 = loop.c
OUT1 = loop.out

# Commands
all: $(OUT1) run 

run: $(OUT1)
	$(MPI) -np $(PROC) ./$(OUT1)

$(OUT1): $(SOURCE1)
	$(LOADER) $(OPT) $(SOURCE1) -o $(OUT1) $(LIB)
	
clean:
	rm $(OUT1)
