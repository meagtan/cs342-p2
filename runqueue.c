#include "runqueue.h"

#include <stdio.h> // testing
#include <stdlib.h>

static const int prio_to_weight[40] = {
/* -20 */     88761,     71755,     56483,     46273,     36291,
/* -15 */     29154,     23254,     18705,     14949,     11916,
/* -10 */      9548,      7620,      6100,      4904,      3906,
/*  -5 */      3121,      2501,      1991,      1586,      1277,
/*   0 */      1024,       820,       655,       526,       423,
/*   5 */       335,       272,       215,       172,       137,
/*  10 */       110,        87,        70,        56,        45,
/*  15 */        36,        29,        23,        18,        15,
};

#define TARGET_LATENCY(q) ((timeunit) GRANULARITY * ((q)->procs > 20 ? (q)->procs : 20))

void rq_init(runqueue *q)
{
	rbtree_init(&q->queue);
	q->running = NULL;
	q->min_vruntime = 0;
	q->procs = 0;
	q->load = 0;
}

// if queue is empty
int rq_empty(runqueue *q)
{
	return !q->running && rbtree_empty(&q->queue);
}

// add process to queue
void rq_add(runqueue *q, pcb *p)
{
	// vruntime = min_vruntime - 10 ms for new process
	// vruntime = min(old_vruntime, min_vruntime - targeted_latency) after io burst
	// need checks, otherwise unsigned subtraction overflows
	if (p->vruntime == 0 && q->min_vruntime > GRANULARITY) // new process
		p->vruntime = q->min_vruntime - GRANULARITY;
	else if (p->bursttime == 0 && q->min_vruntime >= TARGET_LATENCY(q) && p->vruntime < q->min_vruntime - TARGET_LATENCY(q))
		p->vruntime = q->min_vruntime - TARGET_LATENCY(q);
	rbtree_add(&q->queue, p, p->vruntime);
	q->procs++;
	q->load += prio_to_weight[p->prio];
}

void rq_yield(runqueue *q)
{
	if (q->running) {
		q->procs--;
		q->load -= prio_to_weight[q->running->prio];
		q->running = NULL;
	}
}

// update vruntime of running process
void rq_update(runqueue *q, timeunit dt)
{
	// can overflow
	q->running->runtime += dt;
	// q->running->vruntime = exp(log(q->running->runtime) + log(prio_to_weight[20]) - log(prio_to_weight[q->running->prio]));
	q->running->vruntime += dt * prio_to_weight[20] / prio_to_weight[q->running->prio];

	// min_vruntime = max(min_vruntime, min(running.vruntime, queue.min.vruntime))
	timeunit vruntime = q->running->vruntime;
	if (q->queue.min && vruntime > q->queue.min->key)
		vruntime = q->queue.min->key;
	if (q->min_vruntime < vruntime)
		q->min_vruntime = vruntime;
}

// if running process exceeds its time slice (recomputed), preempt and add it to run queue
pcb *rq_preempt(runqueue *q, timeunit t)
{
	q->running->timeslice = TARGET_LATENCY(q) * prio_to_weight[q->running->prio] / q->load;
	if (t >= q->running->burststart + q->running->timeslice) { // end = last time q->running started running after waiting
		pcb *running = q->running;
		rq_yield(q); // remove running process
		rq_add(q, running); // add it back to run queue
		return running;
	}
	return NULL;
}

// schedule new process and set it to running, null if queue empty
void rq_sched(runqueue *q)
{
	if (q->queue.min) {
		q->running = rbtree_pop(&q->queue);
		q->running->timeslice = TARGET_LATENCY(q) * prio_to_weight[q->running->prio] / q->load;
		// printf("targeted latency: %lld load: %d\n", TARGET_LATENCY(q), q->load);
	}
}

void rq_free(runqueue *q)
{
	rbtree_free(&q->queue, 0);
}
