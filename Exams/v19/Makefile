CC := gcc
MPICC := mpicc
FLAGS = -Wall -lm -fopenmp

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))

.PHONY : all clean

all: $(PROGRAMS)

task5_serial : task5_serial.c
	$(CC) $(FLAGS) $< -o $@ $(LDFLAGS)

task5_openmp : task5_openmp.c
	$(CC) $(FLAGS) $< -o $@ $(LDFLAGS)

task5_mpi : task5_mpi.c
	$(MPICC) $(FLAGS) $< -o $@ $(LDFLAGS)

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)