# Cpmpiller
CCOMP = mpicc
LOADER = mpicc
MPI = mpiexec -oversubscribe

# Options
OPT = -g -DDEBUG
LIB = -lm
PROC = 6

# Files
SOURCE1 = pi.c
OUT1 = pi.out

# Commands
all: $(OUT1) run 

run: $(OUT1)
	$(MPI) -np $(PROC) ./$(OUT1)

$(OUT1): $(SOURCE1)
	$(LOADER) $(OPT) $(SOURCE1) -o $(OUT1) $(LIB)
	
clean:
	rm $(OUT1)
