// no need to hide implementation
typedef struct _pcb {
	int pid;
	int prio;
	int start;
	int *cpubursts, *iobursts;
	int burstlen, maxlen; // len(cpubursts) == len(iobursts) + 1 == burstlen < maxlen
} pcb;

void pcb_init(pcb *, int pid);

void pcb_add_cpuburst(pcb *, int);

void pcb_add_ioburst(pcb *, int);

void pcb_free(pcb *);
