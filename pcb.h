/*
 * CS 342 Project 2
 * Ata Deniz Aydin
 * 21502637
 *
 * Declaration and functions for struct storing information about processes.
 */

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

	// statistical variables, updated dynamically
	int burstnum;               // number of cpu bursts completed
	timeunit bursttime;         // currently elapsed time of last cpu burst, <= cpubursts[burstnum]
	timeunit actual_start, end; // turnaround = end - start, end = time process last arrives, starts or pauses execution
	timeunit response;          // total response time, updated whenever process arrives and is scheduled to run
	timeunit waiting;           // total waiting time, updated using end whenever process is scheduled
	timeunit runtime;           // total run time
	timeunit entrance;          // last time process arrived, in the beginning or after I/O burst
	timeunit burststart;        // time the process has last started execution
	timeunit iotime;            // time spent in io bursts

	// other variables for scheduling
	timeunit vruntime;  // virtual run time
	timeunit timeslice; // current time slice of process, may not be necessary to keep here
} pcb;

void pcb_init(pcb *, int pid);

// add burst of given duration
void pcb_add_cpuburst(pcb *, timeunit);
void pcb_add_ioburst(pcb *, timeunit);

void pcb_free(pcb *);

#endif
