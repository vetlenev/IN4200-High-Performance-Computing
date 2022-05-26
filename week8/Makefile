CC := gcc
FLAGS = -lm

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))

.PHONY : all clean

all: $(PROGRAMS)

ex3 : ex3.c 
	$(CC) $< -o $@ -fopenmp

ex_extra : ex_extra.c
	$(CC) $< -o $@ $(FLAGS)

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)