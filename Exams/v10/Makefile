CC := gcc
MPICC := mpicc
FLAGS = -Wall -lm -fopenmp

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))

.PHONY : all clean

all: $(PROGRAMS)

ex3 : ex3.c
	$(CC) $(FLAGS) $< -o $@ $(LDFLAGS)

ex4 : ex4.c
	$(MPICC) $(FLAGS) $< -o $@ -lm

ex5 : ex5.c
	$(CC) $(FLAGS) $< -o $@ -lm

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)