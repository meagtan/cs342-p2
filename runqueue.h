#ifndef __RUNQUEUE_H
#define __RUNQUEUE_H

#include "rbtree.h"

#define GRANULARITY 10000000

typedef struct _runqueue {
	rbtree queue;
	pcb *running;
	int min_vruntime;
	int procs;
	int load;
} runqueue;

void rq_init(runqueue *);

// if queue is empty
int rq_empty(runqueue *);

// add process to queue
void rq_add(runqueue *, pcb *);

// remove running process
void rq_yield(runqueue *);

// update vruntime of running process and change its position in runqueue
void rq_update(runqueue *, int);

// if running process exceeds its time slice, preempt and add it to run queue
// return value is running process if preempted, NULL otherwise
pcb *rq_preempt(runqueue *, int t);

// schedule new process and set it to run, null if queue empty
void rq_sched(runqueue *);

void rq_free(runqueue *);

#endif
