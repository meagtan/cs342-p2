#include "rbtree.h"

extern const int prio_to_weight[40];

typedef struct {
	rbtree queue;
	pcb *running;
	double min_vruntime;
	int procs;
	double load;
} runqueue;

void rq_init(runqueue *);

// if queue is empty
int rq_empty(runqueue *);

// schedule new process and set it to run, null if queue empty
void rq_sched(runqueue *);

// add process to queue
void rq_add(runqueue *, pcb *);

void rq_free(runqueue *);
