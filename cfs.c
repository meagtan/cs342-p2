#include <stdio.h>
#include <string.h>

#include "runqueue.h"

#define GRANULARITY 10000000

int main(int argc, char *argv[])
{
	int *turnaround, waiting, response; // stats
	runqueue q;
	rbtree procs; // stores all processes
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

	// go through each line in file
	int pid, prio, burst;
	char buf[8];
	pcb *proc;
	while (!feof(f)) {
		fscanf(f, "%d %s", &pid, buf);
		if (!strcmp(buf, " start")) {
			pcb_init(proc, pid);
			fscanf(f, " %d prio %d\n", &proc->start, &proc->prio);
			rbtree_add(&procs, proc, proc->start);
		} else {
			proc = rbtree_get(&procs, pid);
			fscanf(f, "%d\n", &burst);
			if (!strcmp(buf, "cpu"))
				pcb_add_cpuburst(proc, burst);
			else
				pcb_add_ioburst(proc, burst);
		}
	}

	pcb *min = rbtree_min(&procs); // retrieve first process to execute
	for (int t = 0; min != NULL || !rq_empty(&rq); t += GRANULARITY) {
		// add new processes to ready queue
		while (min != NULL && min->start * 1000000 <= t) {
			rq_add(&rq, min); // also calculates vruntime
			min = rbtree_succ(&procs, min); // in-order successor
		}

		// update time of running process if not null
		if (q.running != NULL) {
			q.running->runtime += GRANULARITY;
			// increment other properties, such as turnaround time

			// recalculate time slice, vruntime, preempt
		}

		// if no running process, schedule
		if (q.running == NULL) {
			rq_sched(&rq);

			// update statistics such as waiting time, response time
		}
	}

	// output statistics

	// write log file

	rq_free(&q);
	rbtree_free(&procs, 1); // free each pcb as well
}
