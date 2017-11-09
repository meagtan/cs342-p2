#include <stdlib.h>
#include <stdio.h>

#include "pcb.h"

#define MAXLEN 10

void pcb_init(pcb *p, int pid)
{
	p->pid = pid;
	p->cpubursts = malloc(MAXLEN * sizeof(timeunit));
	p->iobursts  = malloc(MAXLEN * sizeof(timeunit));
	p->burstlen  = 0; // number of cpu bursts
	p->maxlen    = MAXLEN;

	p->actual_start = p->end = 0;
	p->waiting = p->runtime = 0;

	p->vruntime = p->timeslice = p->burstnum = p->bursttime = p->burststart = 0;
}

void pcb_add_cpuburst(pcb *p, timeunit burst)
{
	if (p->burstlen == p->maxlen) {
		p->maxlen <<= 1;
		p->cpubursts = realloc(p->cpubursts, p->maxlen * sizeof(timeunit));
		p->iobursts = realloc(p->iobursts, p->maxlen * sizeof(timeunit));
		if (p->iobursts == NULL)
			fprintf(stderr, "malloc failed\n");
	}
	p->cpubursts[p->burstlen++] = burst;
}

void pcb_add_ioburst(pcb *p, timeunit burst)
{
	/*
	if (p->burstlen == p->maxlen) {
		p->maxlen <<= 1;
	}
	*/
	p->iobursts[p->burstlen-1] = burst;
}

void pcb_free(pcb *p)
{
	free(p->cpubursts);
	free(p->iobursts);
	free(p);
}
