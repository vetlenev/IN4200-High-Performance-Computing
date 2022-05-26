CC := gcc
MPICC := mpicc
FLAGS = -Wall -lm

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))

.PHONY : all clean

all: $(PROGRAMS)

ex1 : ex1.c 
	$(MPICC) $< -o $@ $(FLAGS)

ex2 : ex2.c 
	$(MPICC) $< -o $@ $(FLAGS)

ex3_serial : ex3_serial.c 
	$(MPICC) $< -o $@ $(FLAGS)

ex3_parallel : ex3_parallel.c 
	$(MPICC) $< -o $@ $(FLAGS)

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)