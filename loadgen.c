#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


// sample from exponential distribution with mean m
#define EXP(m) ((m) * -log(rand() / (RAND_MAX + 1.)))

#define ROUND(n, d) (d * ((int) (n / d)))

int main(int argc, char *argv[])
{
	int N, start, burst, cpu, io;

	if (argc != 7) {
		fprintf(stderr, "Format: loadgen <N> <avg_start_time> <avg_num_bursts> <avg_cpu_len> <avg_io_len> <workloadfile>\n");
		return 1;
	}

	N     = atoi(argv[1]);
	start = atoi(argv[2]);
	burst = atoi(argv[3]);
	cpu   = atoi(argv[4]);
	io    = atoi(argv[5]);

	FILE *f = fopen(argv[6], "w");
	if (!f) {
		fprintf(stderr, "Error: could not open file %s.\n", argv[6]);
		return 1;
	}

	// seed rng
	srand(time(NULL));

	int bursttime;
	// generate workload for each process
	for (int i = 1; i <= N; ++i) {
		while (!(bursttime = EXP(burst))); // burst times should be positive
		// generate start time
		// source: en.wikipedia.org/wiki/Exponential_distribution#Generating_exponential_variates
		fprintf(f, "%d  start %d prio %d\n", i, ROUND((int) EXP(start), 10), rand() % 40);

		// generate number of bursts
		for (int j = bursttime; j; --j) {
			// generate cpu and io bursts
			fprintf(f, "%d cpu %d\n", i, ROUND(EXP(cpu), 10));
			fprintf(f, "%d io %d\n",  i, ROUND(EXP(io), 10));
		}
		fprintf(f, "%d cpu %d\n", i, ROUND(EXP(cpu), 10));
	}
	fclose(f);
}
