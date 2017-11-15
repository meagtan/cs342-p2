#include "event.h"

void evt_init(evtqueue *q)
{
	rbtree_init(&q->arrivals);
	q->finish = IDLE; // no process currently running, finish event has least priority
	q->last = 0;
}

// add process to event queue, indexed by time of arrival
void evt_addproc(evtqueue *q, pcb *p, timeunit t)
{
	rbtree_add(&q->arrivals, p, t);
}

// time of next event
timeunit evt_min(evtqueue *q)
{
	timeunit t = (q->last / GRANULARITY + 1) * GRANULARITY; // next timer tick
	if (q->finish <= t)
		t = q->finish;
	node *min = rbtree_min(&q->arrivals);
	if (min && min->key <= t)
		t = min->key;
	return t;
}

// retrieve next event, pass time of occurrence in parameter, return type of event in return value
// TODO if two events occur at the same time, scheduler should take both into account
int evt_pop(evtqueue *q, timeunit *t, pcb **p)
{
	int res = 0;
	*t = (q->last / GRANULARITY + 1) * GRANULARITY; // next multiple of GRANULARITY
	if (q->finish <= *t) { // never true for q->finish == -1, as both arguments are unsigned
		*t = q->finish;
		res = FINISH;
	}

	// retrieve min, compare with *t
	node *min = rbtree_min(&q->arrivals);
	if (min && min->key <= *t) {
		*t = min->key;
		*p = rbtree_pop(&q->arrivals);
		res = ARRIVAL;
	}

	q->last = *t;
	return res;
}

void evt_setfinish(evtqueue *q, pcb *p, timeunit t)
{
	q->finish = t + p->cpubursts[p->burstnum] - p->bursttime;
}

// no process to arrive or finish execution
int evt_empty(evtqueue *q)
{
	return rbtree_empty(&q->arrivals) && q->finish == IDLE;
}

void evt_free(evtqueue *q)
{
	rbtree_free(&q->arrivals, 0);
}

