CC := gcc
MPICC := mpicc
FLAGS = -Wall -lm -fopenmp

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))

.PHONY : all clean

all: $(PROGRAMS)

ex2 : ex2.c
	$(CC) $(FLAGS) $< -o $@ $(LDFLAGS)

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)