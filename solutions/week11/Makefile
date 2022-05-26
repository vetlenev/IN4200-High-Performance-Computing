CC := gcc
MPICC := mpicc
FLAGS = -Wall -g -Og

PROGRAMS = w11_e1_pingpong w11_e2_trapezoidal w11_e3_wave
DEP = set11.h

.PHONY : all clean

all: $(PROGRAMS)

w11_e1_pingpong : w11_e1_pingpong.c
	$(MPICC) $< -o $@ $(FLAGS) -std=c99

w11_e2_trapezoidal : w11_e2_trapezoidal.c
	$(MPICC) $< -o $@ $(FLAGS) -std=c99 -lm # math

w11_e3_wave : w11_e3_wave.c
	$(MPICC) $< -o $@ $(FLAGS) -lm

README.pdf : README.md
	pandoc -o $@ $<

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)
