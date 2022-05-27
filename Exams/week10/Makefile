CC := gcc
MPICC := mpicc
FLAGS = -Wall

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))
DEP = matvec.h

.PHONY : all clean

all: $(PROGRAMS)

ex2 : ex2.c
	$(MPICC) $< -o $@ $(FLAGS)

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)