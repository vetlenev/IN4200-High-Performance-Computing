CC := gcc
PROFILE := -pf 

PROGRAMS = $(patsubst %.c, %.exe, $(wildcard *.c))
DEP = week5.h

all: $(PROGRAMS) # dynamically makes all programs provided as arguments

ex1.exe: ex1.c $(DEP) # < and @ dynamically picks files provided by wildcard in PROGRAMS
	$(CC) $< -o $@ ex1.o

ex3.exe: ex3.c $(DEP)
	$(CC) $< -o $@ ex3.o