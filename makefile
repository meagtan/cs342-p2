all: loadgen.c cfs.c rbtree.c pcb.c runqueue.c
	cc -o loadgen loadgen.c -lm
	cc -o cfs cfs.c rbtree.c pcb.c runqueue.c
