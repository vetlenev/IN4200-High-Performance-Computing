CC := gcc
FLAGS = -Wall -lm

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))

.PHONY : all clean

all: $(PROGRAMS)

ex2 : ex2.c 
	$(CC) $< -o $@ -fopenmp $(FLAGS)

ex3 : ex3.c 
	$(CC) $< -o $@ -fopenmp $(FLAGS)

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)