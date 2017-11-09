#ifndef __RUNQUEUE_H
#define __RUNQUEUE_H

#include "rbtree.h"
#include "specs.h"

typedef struct _runqueue {
	rbtree queue;
	pcb *running;
	timeunit min_vruntime;
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
// time argument is the duration the process has run until last time scheduler ran
void rq_update(runqueue *, timeunit);

// if running process exceeds its time slice, preempt and add it to run queue
// return value is running process if preempted, NULL otherwise
pcb *rq_preempt(runqueue *, timeunit);

// schedule new process and set it to run, null if queue empty
void rq_sched(runqueue *);

void rq_free(runqueue *);

#endif
