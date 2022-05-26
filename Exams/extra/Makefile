CC := gcc
MPICC := mpicc
FLAGS = -Wall -lm -fopenmp

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))
DEP = utility.h

.PHONY : all clean

all: $(PROGRAMS)

standard_dev : standard_dev.c $(DEP)
	$(MPICC) $(FLAGS) $< -o $@ $(FLAGS)

wave2D : wave2D.c
	$(MPICC) $(FLAGS) $< -o $@ $(FLAGS)

block2D_decomp : block2D_decomp.c
	$(CC) $< -o $@ $(FLAGS)

mmm : mmm.c
	$(MPICC) $< -o $@ $(FLAGS)

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)