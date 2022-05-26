CC := gcc
MPICC := mpicc
FLAGS = -Wall

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))
DEP = week10.h

.PHONY : all clean

all: $(PROGRAMS)

ex1 : ex1.c 
	$(MPICC) $< -o $@ $(FLAGS)

ex2 : ex2.c $(DEP)
	$(MPICC) $< -o $@ $(FLAGS)

ex3 : ex3.c 
	$(MPICC) $< -o $@ $(FLAGS)

ex4 : ex4.c
	$(MPICC) $< -o $@ $(FLAGS)

ex4_new : ex4_new.c $(DEP)
	$(MPICC) $< -o $@ $(FLAGS)	

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)