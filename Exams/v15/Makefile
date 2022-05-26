CC := gcc
MPICC := mpicc
FLAGS = -Wall -lm -fopenmp

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))

.PHONY : all clean

all: $(PROGRAMS)

ex4 : ex4.c
	$(CC) $< -o $@ $(FLAGS)

ex5 : ex5.c
	$(CC) $< -o $@ $(FLAGS)

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)