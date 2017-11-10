#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runqueue.h"
#include "event.h"

int main(int argc, char *argv[])
{
	// int *turnaround, waiting, response; // stats
	runqueue q;
	rbtree procs,   // stores all processes sorted by pid
	       waiting; // stores processes currently waiting to enter run queue (either not started or in io burst) // evtqueue event;
	evtqueue event;
	FILE *f;
	node *min;

	// read workload file
	if (argc != 3) {
		fprintf(stderr, "Format: cfs <workload file> <output file>\n");
		return 1;
	}

	f = fopen(argv[1], "r");
	if (!f) {
		fprintf(stderr, "Error: could not open workload file %s.\n", argv[1]);
		return 1;
	}

	rq_init(&q);
	rbtree_init(&procs);
	// rbtree_init(&waiting);
	evt_init(&event);

	// go through each line in file
	int pid, prio;
	timeunit burst;
	char buf[8];
	pcb *proc;
	while (!feof(f)) {
		fscanf(f, "%d %s", &pid, buf);
		if (!strcmp(buf, "start")) {
			// create pcb
			proc = malloc(sizeof(pcb));
			if (proc == NULL) printf("malloc failed\n");
			pcb_init(proc, pid);
			fscanf(f, " %llu prio %d\n", &proc->start, &proc->prio);
			proc->start *= 1000000; // convert to ns

			// add pcb to process list and create entrance event
			rbtree_add(&procs, proc, pid);
			// rbtree_add(&waiting, proc, proc->start);
			evt_addproc(&event, proc, proc->start);
		} else {
			proc = rbtree_get(&procs, pid); // assuming workload file in correct format
			fscanf(f, " %llu\n", &burst); // skip newline, otherwise last line read twice
			burst *= 1000000;

			if (!strcmp(buf, "cpu"))
				pcb_add_cpuburst(proc, burst);
			else
				pcb_add_ioburst(proc, burst);
		}
	}

	// loop until no processes waiting, running or ready
	// node *min = rbtree_min(&waiting); // retrieve first process to execute
	// for (long long t = 0; (min = rbtree_min(&waiting)) != NULL || !rq_empty(&q); t += GRANULARITY) {
	timeunit t, t1; // t1 - time of previous event
	while (!evt_empty(&event) || !rq_empty(&q)) {
		// condition on type of event occurring
		// if arrival, add process to run queue and then schedule
		// if finish, remove running process, add to event queue, update statistics, then schedule
		// if timer tick, just schedule
		switch (evt_pop(&event, &t, &proc)) {
		case ARRIVAL:
			printf("time: %llu pid: %d arrival\n", t / 1000000, proc->pid);
			rq_add(&q, proc); // also calculates vruntime
			break;
		case FINISH: // does this keep the changes in q.running in mind?
			// TODO update statistics
			rq_update(&q, t - q.running->end);
			q.running->bursttime = 0;
			q.running->end = t;

			// if process not in last cpu burst, should enter io burst and enter run queue afterwards
			if (q.running->burstnum < q.running->burstlen - 1) {
				printf("time: %llu pid: %d enter io burst %d of length %llu\n", t / 1000000, q.running->pid,
					q.running->burstnum, q.running->iobursts[q.running->burstnum] / 1000000); // testing
				// rbtree_add(&waiting, q.running, t + q.running->iobursts[q.running->burstnum]);
				evt_addproc(&event, q.running, t + q.running->iobursts[q.running->burstnum]);
				q.running->burstnum++;
			} else printf("time: %llu pid: %d exit\n", t / 1000000, q.running->pid); // testing

			rq_yield(&q); // remove q.running from run queue
			event.finish = IDLE;
			break;
		}

		// schedule process

		// update and possibly preempt running process if not null
		if (q.running != NULL) {
			// update virtual runtime, possibly preempt running process
			rq_update(&q, t - q.running->end);
			q.running->bursttime += t - q.running->end;
			q.running->end = t;

			// if running process preempted, in which case proc = q->running, q->running = NULL
			if (proc = rq_preempt(&q, t)) {
				event.finish = IDLE;

		// testing
		node *min = rbtree_min(&q.queue);
		while (min != NULL) {
			printf("%d %llu\t", min->process->pid, min->process->vruntime / 1000000);
			min = min->next;
		}
		printf("\n");

				printf("time: %llu pid: %d preempted, new timeslice %llu\n", t / 1000000, proc->pid, proc->timeslice / 1000000); // testing
			}
		}

		// schedule if running process null, preempted or finished burst
		if (q.running == NULL) {
			rq_sched(&q);

			// running process newly scheduled
			// update statistics such as waiting time, response time
			if (q.running) {
				if (!q.running->actual_start) // first added
					q.running->actual_start = t;
				q.running->burststart = t;
				q.running->end = t; // should not affect last value of end
				// set finishing time of running process
				evt_setfinish(&event, q.running, t);
			}

			// testing, write to log file instead
			if (q.running)
				printf("time: %llu running: %d bursttime: %llu burst: %llu timeslice: %llu\n", t / 1000000, q.running->pid,
					q.running->bursttime / 1000000, q.running->cpubursts[q.running->burstnum] / 1000000, q.running->timeslice / 1000000);
		}

		/*
		// add new processes to run queue
		// clean up loop later
		while ((min = rbtree_min(&waiting)) != NULL && min->process->start <= t) {
			rbtree_pop(&waiting);
			// min = rbtree_min(&waiting); // in-order successor
		}

		// update time of running process if not null
		if (q.running != NULL) {

			// increment other properties, such as turnaround time
			// better calculated from difference of starting and ending time

			// update virtual runtime, possibly preempt
			rq_update(&q, GRANULARITY);

			// if no running process,
			// or if any process added recently has higher priority than running process,
			// or if running process exceeded its time slice, preempt and invoke scheduler
			if (q.running->bursttime >= q.running->cpubursts[q.running->burstnum]) {
			} else
		}
		*/

	}

	// output statistics

	// write log file

	rq_free(&q);
	// rbtree_free(&waiting, 0);
	evt_free(&event);
	rbtree_free(&procs, 1); // free each pcb as well
}
