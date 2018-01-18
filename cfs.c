/*
 * CS 342 Project 2
 * Ata Deniz Aydin
 * 21502637
 *
 * Simulates CFS scheduling algorithm.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runqueue.h"
#include "event.h"

#define MIL 1000000

// milliseconds, usage: a MS = a / MIL
#define MS / MIL

int main(int argc, char *argv[])
{
	runqueue q;
	rbtree procs;   // stores all processes sorted by pid
	evtqueue event; // stores all external events for the scheduler to respond to
	FILE *f;
	timeunit idletime = 0; // last time CPU became idle

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
	evt_init(&event);

	// go through each line in file
	int pid, prio;
	timeunit burst;
	char buf[8];
	pcb *proc;
	while (!feof(f)) {
		// read pid, keyword
		fscanf(f, " %d %s", &pid, buf);
		if (!strcmp(buf, "start")) { // if start, create pcb
			proc = malloc(sizeof(pcb));
			pcb_init(proc, pid);

			fscanf(f, " %llu", &proc->start);
			fscanf(f, " prio %d\n", &proc->prio);
			proc->start *= MIL; // convert to ns

			// add pcb to process list and create entrance event
			rbtree_add(&procs, proc, pid);
			evt_addproc(&event, proc, proc->start);
		} else if (strcmp(buf, "end")) { // if not end, add burst
			// proc = rbtree_get(&procs, pid); // assuming workload file in correct format
			fscanf(f, " %llu\n", &burst); // skip newline, otherwise last line read twice
			burst *= MIL;

			if (!strcmp(buf, "cpu"))
				pcb_add_cpuburst(proc, burst);
			else
				pcb_add_ioburst(proc, burst);
		}
	}

	// close input, open output
	fclose(f);
	f = fopen(argv[2], "w");

	// loop until no processes waiting, running or ready
	timeunit t; // time of current event
	while (!evt_empty(&event) || !rq_empty(&q)) {
		// process all events at same time instant
		do {
			// condition on type of event occurring
			// if arrival, add process to run queue and then schedule
			// if finish, remove running process, add to event queue, update statistics, then schedule
			// if timer tick, just schedule
			switch (evt_pop(&event, &t, &proc)) {
			case ARRIVAL:
				proc->entrance = t; // update last time process arrived
				proc->end = t;      // update last time process was handled by scheduler
				rq_add(&q, proc);   // add process to runqueue, calculating vruntime

				// printf("time: %llu pid: %d arrival\n", t MS, proc->pid);
				// if (q.running && q.running->vruntime > proc->vruntime) printf("preempt\n");
				break;
			case FINISH:
				// write execution time of running process to log
				fprintf(f, "%d %llu\n", q.running->pid, (t - q.running->burststart) MS);

				// update vruntime, bursttime etc.
				rq_update(&q, t);
				q.running->bursttime = 0;

				// if process not in last cpu burst, should enter io burst and enter run queue afterwards
				if (q.running->burstnum < q.running->burstlen - 1) {
					// printf("time: %llu pid: %d enter io burst %d of length %llu\n", t MS, q.running->pid,
					// 	q.running->burstnum, q.running->iobursts[q.running->burstnum] MS); // testing
					q.running->iotime += q.running->iobursts[q.running->burstnum];
					evt_addproc(&event, q.running, t + q.running->iobursts[q.running->burstnum]);
					q.running->burstnum++;
				}
				// else printf("time: %llu pid: %d exit\n", t MS, q.running->pid); // testing

				rq_yield(&q); // remove q.running from run queue

				// processor currently idle (another process may be scheduled, or not)
				event.finish = IDLE;
				idletime = t;
				break;
			}
		} while (evt_min(&event) == t);

		// update and possibly preempt running process if not null
		if (q.running != NULL) {
			// update virtual runtime, possibly preempt running process
			rq_update(&q, t);

			// if running process preempted, in which case proc = q->running, q->running = NULL
			if (proc = rq_preempt(&q, t)) {
				// processor currently idle
				event.finish = IDLE;
				idletime = t;

				// log last execution time of previously running process
				fprintf(f, "%d %llu\n", proc->pid, (t - proc->burststart) MS);

/*
				// testing
				node *min = rbtree_min(&q.queue);
				while (min != NULL) {
					printf("%d %llu\t", min->process->pid, min->process->vruntime MS);
					min = min->next;
				}
				printf("\n");
*/

				// printf("time: %llu pid: %d preempted, new timeslice %llu\n", t MS, proc->pid, proc->timeslice MS); // testing
			}
		}

		// schedule if running process null, preempted or finished burst
		if (q.running == NULL) {
			rq_sched(&q);

			// running process newly scheduled
			// update statistics such as waiting time, response time
			if (q.running) {
				if (!q.running->bursttime) // newly scheduled after arrival or io burst
					q.running->response += t - q.running->entrance;
				if (!q.running->runtime) // first added
					q.running->actual_start = t;
				q.running->burststart = t;
				q.running->waiting += t - q.running->end; // end last updated when running arrived or was preempted
				q.running->end = t;

				// set finishing time of running process
				evt_setfinish(&event, q.running, t);

				// if CPU was previously idle, record it
				// idletime is updated whenever running process leaves cpu
				if (t - idletime)
					fprintf(f, "idle %llu\n", (t - idletime) MS);
			}

			// testing
			// if (q.running)
			// 	printf("time: %llu running: %d bursttime: %llu burst: %llu timeslice: %llu\n", t MS, q.running->pid,
			// 		q.running->bursttime MS, q.running->cpubursts[q.running->burstnum] MS, q.running->timeslice MS);
		}
	}

	// output statistics
	// iteration should also work if rbtree changed to an actual red-black tree implementation,
	// in which case succ would return the in-order successor
	for (node *min = rbtree_min(&procs); min; min = rbtree_succ(&procs, min)) {
		proc = min->process;
		printf("%d %d %llu %llu %llu %llu %llu\n", proc->pid, proc->prio, proc->start MS, proc->end MS,
		       (proc->end - proc->start) MS, proc->waiting MS, proc->response / proc->burstlen MS);
	}

	rq_free(&q);
	evt_free(&event);
	rbtree_free(&procs, 1); // free each pcb as well

	fclose(f);
}
