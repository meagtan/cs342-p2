// no need to hide implementation
typedef struct _pcb {
	int pid;
	int prio;
	int start;
	int *cpubursts, *iobursts;
	int burstlen, maxlen; // len(cpubursts) == len(iobursts) + 1 == burstlen < maxlen
} pcb;

pcb_init(pcb *);

pcb_add_cpuburst(pcb *, int);

pcb_add_ioburst(pcb *, int);
