#ifndef __PCB_H
#define __PCB_H

#include "specs.h"

// no need to hide implementation
// time variables all in nanoseconds
typedef struct _pcb {
	int pid;
	int prio;
	timeunit start;
	timeunit *cpubursts, *iobursts;
	int burstlen, maxlen; // len(cpubursts) == len(iobursts) + 1 == burstlen < maxlen

	// statistics
	timeunit actual_start, end; // turnaround = end - actual_start, end = time process last enters or leaves execution
	timeunit response; // average response time, updated whenever process arrives and is scheduled to run
	timeunit waiting;
	timeunit runtime; // total run time
	timeunit entrance;

	// other variables for scheduling
	timeunit vruntime;  // virtual run time
	timeunit timeslice; // current time slice of process, may not be necessary to keep here
	int burstnum;  // number of cpu bursts completed
	timeunit bursttime; // currently elapsed time of last cpu burst, <= cpubursts[burstnum]
	timeunit burststart; // time the process has last started execution
} pcb;

void pcb_init(pcb *, int pid);

void pcb_add_cpuburst(pcb *, timeunit);

void pcb_add_ioburst(pcb *, timeunit);

void pcb_free(pcb *);

#endif
