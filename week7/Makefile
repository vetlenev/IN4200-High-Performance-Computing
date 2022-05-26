CC := gcc
FLAGS = -lm

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))
DEP = matvec.h

.PHONY : all clean

all: $(PROGRAMS)

ex1 : ex1.c 
	$(CC) $< -o $@ -fopenmp $(FLAGS)

ex2 : ex2.c 
	$(CC) $< -o $@ -fopenmp

ex3 : ex3.c 
	$(CC) $< -o $@ -fopenmp $(DEP)

ex3c : ex3c.c 
	$(CC) $< -o $@ -fopenmp $(DEP)

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)