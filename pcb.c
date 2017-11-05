#include <stdlib.h>
#include <stdio.h>

#include "pcb.h"

#define MAXLEN 10

void pcb_init(pcb *p, int pid)
{
	p->pid = pid;
	p->cpubursts = malloc(MAXLEN * sizeof(int));
	p->iobursts  = malloc(MAXLEN * sizeof(int));
	p->burstlen  = 0; // number of cpu bursts
	p->maxlen    = MAXLEN;
}

void pcb_add_cpuburst(pcb *p, int burst)
{
	if (p->burstlen == p->maxlen) {
		p->maxlen <<= 1;
		p->cpubursts = realloc(p->cpubursts, p->maxlen * sizeof(int));
		p->iobursts = realloc(p->iobursts, p->maxlen * sizeof(int));
		if (p->iobursts == NULL)
			fprintf(stderr, "malloc failed\n");
	}
	p->cpubursts[p->burstlen++] = burst;
}

void pcb_add_ioburst(pcb *p, int burst)
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
