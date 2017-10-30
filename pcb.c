#include <stdlib.h>

#include "pcb.h"

#define MAXLEN 10

void pcb_init(pcb *p)
{
	p->cpubursts = malloc(MAXLEN * sizeof(int));
	p->iobursts  = malloc(MAXLEN * sizeof(int));
	p->burstlen  = 0;
	p->maxlen    = MAXLEN;
}

void pcb_add_cpuburst(pcb *p, int burst)
{
	if (p->burstlen == p->maxlen) {
		p->maxlen <<= 1;
		p->cpubursts = realloc(p->maxlen * sizeof(int));
	}
	p->cpubursts[p->burstlen++] = burst;
}

void pcb_add_ioburst(pcb *p, int burst)
{
	if (p->burstlen == p->maxlen) {
		p->maxlen <<= 1;
		p->iobursts = realloc(p->maxlen * sizeof(int));
	}
	p->iobursts[p->burstlen++] = burst;
}
