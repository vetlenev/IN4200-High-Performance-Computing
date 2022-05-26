CC := gcc
MPICC := mpicc
FLAGS = -Wall -lm -fopenmp

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))

.PHONY : all clean

all: $(PROGRAMS)

ex3_omp : ex3_omp.c
	$(CC) $(FLAGS) $< -o $@ -lm

ex3_mpi : ex3_mpi.c
	$(MPICC) $(FLAGS) $< -o $@ -lm

ex4_omp : ex4_omp.c
	$(CC) $(FLAGS) $< -o $@ -lm

ex5_gauss : ex5_gauss.c
	$(CC) $(FLAGS) $< -o $@ -lm

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)