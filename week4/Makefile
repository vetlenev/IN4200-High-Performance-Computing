CC := gcc # compilation service to use
PROFILE := -pg # profiling service

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c)) ex1_fast.exe
DEP = week4.h # dependency for programs

.PHONY : all clean

all: $(PROGRAMS)

ex1.exe : ex1.c $(DEP)
	$(CC) $< -o $@ -lm ex1.o

ex1_fast.exe : ex1.c $(DEP)
	$(CC) $< -o $@ -O3 -lm ex1.o -fopt-info

ex3.exe : ex3.c $(DEP)
	$(CC) $< -o $@ -fopt-info