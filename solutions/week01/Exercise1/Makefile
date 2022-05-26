ex1a:
	gcc ex1a.c -o ex1a

ex1b:

	gcc ex1b.c -o ex1b

ex1c:
	gcc ex1c.c -o ex1c

ex1c_optimised:
	gcc -O3 ex1c.c -o ex1c_optimised

ex1d:
	gcc ex1d.c -o ex1d

all:
	make ex1a ex1b ex1c ex1c_optimised ex1d

remove:
	ls | grep -v "\." | grep -v Makefile |xargs rm
