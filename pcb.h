#ifndef __PCB_H
#define __PCB_H

// no need to hide implementation
// time variables all in nanoseconds
typedef struct _pcb {
	int pid;
	int prio;
	int start;
	int *cpubursts, *iobursts;
	int burstlen, maxlen; // len(cpubursts) == len(iobursts) + 1 == burstlen < maxlen

	// statistics
	int actual_start, end; // turnaround = end - actual_start, end = time process last enters or leaves execution
	int waiting; // total waiting time, updated whenever process is scheduled to run
	int runtime; // total run time

	// other variables for scheduling
	int vruntime;  // virtual run time
	int timeslice; // current time slice of process, may not be necessary to keep here
	int burstnum;  // number of cpu bursts completed
	int bursttime; // currently elapsed time of last cpu burst, <= cpubursts[burstnum]
} pcb;

void pcb_init(pcb *, int pid);

void pcb_add_cpuburst(pcb *, int);

void pcb_add_ioburst(pcb *, int);

void pcb_free(pcb *);

#endif
