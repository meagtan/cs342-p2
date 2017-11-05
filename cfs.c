#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runqueue.h"

int main(int argc, char *argv[])
{
	// int *turnaround, waiting, response; // stats
	runqueue q;
	rbtree procs,   // stores all processes sorted by pid
	       waiting; // stores processes currently waiting to enter run queue (either not started or in io burst)
	FILE *f;

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
	rbtree_init(&waiting);

	// go through each line in file
	// TODO add pcbs to another linked list sorted by pid for correct output
	int pid, prio, burst;
	char buf[8];
	pcb *proc;
	while (!feof(f)) {
		fscanf(f, "%d %s", &pid, buf);
		// printf("%s\n", buf); // testing
		if (!strcmp(buf, "start")) {
			proc = malloc(sizeof(pcb));
			pcb_init(proc, pid);
			fscanf(f, " %d prio %d\n", &proc->start, &proc->prio);
			proc->start *= 1000000; // convert to ns
			// printf("%d  start %d prio %d\n", pid, proc->start, proc->prio); // testing
			rbtree_add(&procs, proc, pid);
			rbtree_add(&waiting, proc, proc->start);
		} else {
			proc = rbtree_get(&procs, pid); // assuming workload file in correct format
			fscanf(f, " %d", &burst);
			burst *= 1000000;
			if (!strcmp(buf, "cpu"))
				pcb_add_cpuburst(proc, burst);
			else
				pcb_add_ioburst(proc, burst);
		}
	}

	node *min = rbtree_min(&waiting); // retrieve first process to execute
	for (int t = 0; min != NULL || !rq_empty(&q); t += GRANULARITY) {
		// add new processes to run queue
		while (min != NULL && min->process->start <= t) {
			rq_add(&q, min->process); // also calculates vruntime
			rbtree_pop(&waiting);
			min = rbtree_min(&waiting); // in-order successor
		}

		// update time of running process if not null
		if (q.running != NULL) {
			q.running->bursttime += GRANULARITY;
			q.running->runtime   += GRANULARITY;

			// increment other properties, such as turnaround time
			// better calculated from difference of starting and ending time

			// update virtual runtime, possibly preempt
			rq_update(&q, GRANULARITY);

			// if no running process,
			// or if any process added recently has higher priority than running process,
			// or if running process exceeded its time slice, preempt and invoke scheduler
			if (q.running->bursttime >= q.running->cpubursts[q.running->burstnum]) {
				q.running->bursttime = 0;
				// if process not in last cpu burst, should enter io burst and enter run queue afterwards
				if (q.running->burstnum < q.running->burstlen - 1) {
					rbtree_add(&waiting, q.running, t + q.running->iobursts[q.running->burstnum]);
					q.running->burstnum++;
				}
				q.running->end = t;
				rq_yield(&q); // remove q.running from run queue
			} else if (proc = rq_preempt(&q, t)) { // if running process preempted, in which case proc = q->running, q->running = NULL
				proc->end = t;
			}
		}

		// schedule if running process null, preempted or finished burst
		if (q.running == NULL) {
			rq_sched(&q);

			// update statistics such as waiting time, response time
			if (q.running) {
				if (!q.running->actual_start) // first added
					q.running->actual_start = t;
				q.running->end = t; // should not affect last value of end
			}

			// testing, write to log file instead
			if (q.running)
				printf("time: %d running: %d bursttime: %d burst: %d timeslice: %d\n", t / 1000000, q.running->pid,
					q.running->bursttime / 1000000, q.running->cpubursts[q.running->burstnum] / 1000000, q.running->timeslice / 1000000);
		}
	}

	// output statistics

	// write log file

	rq_free(&q);
	rbtree_free(&waiting, 0);
	rbtree_free(&procs, 1); // free each pcb as well
}
