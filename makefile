all: loadgen.c
	cc -o loadgen loadgen.c -lm
