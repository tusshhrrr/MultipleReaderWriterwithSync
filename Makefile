all: rw
rw: rw.c
	gcc -o rw  rw.c -lpthread