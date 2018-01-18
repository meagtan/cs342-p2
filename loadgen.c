/*
 * CS 342 Project 2
 * Ata Deniz Aydin
 * 21502637
 *
 * Generates workload file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// sample from uniform distribution on (0,1)
#define UNI() (rand() / (RAND_MAX + 1.))

// sample from -ln U(0,1)
#define LOGUNI() (-log(rand()) + log(RAND_MAX + 1.))

// sample from exponential distribution with mean m
// source: en.wikipedia.org/wiki/Exponential_distribution#Generating_exponential_variates
#define EXP(m) ((int) ((m) * LOGUNI()))

// sample from normal distribution with mean m and std s
// Box-Muller transform
#define NORMAL(m, s) ((int) (m + s * sqrt(2 * LOGUNI()) * cos(2*M_PI*UNI())))

// largest integer multiple of d smaller than n, for positive arguments at least
#define ROUND(n, d) (d * ((int) (n / d)))

// generate var using the first nonzero value expr returns
#define GENERATE(var, expr) while (!(var = (expr)))

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

	// printf("N: %d start: %d burst: %d cpu: %d io: %d\n", N, start, burst, cpu, io);

	FILE *f = fopen(argv[6], "w");
	if (!f) {
		fprintf(stderr, "Error: could not open file %s.\n", argv[6]);
		return 1;
	}

	// seed rng
	srand(time(NULL));

	int burstnum, starttime, bursttime; // generated number of bursts, start time, burst time
	// generate workload for each process
	for (int i = 1; i <= N; ++i) {
		// generate number of bursts
		// equivalent to sampling bursttime from a geometric distribution with success probability 1-exp(-burst):
		// integrating burst*exp(-burst*t) for n<t<n+1 (due to rounding) gives (1-exp(-burst))*exp(-burst)^n
		GENERATE(burstnum, EXP(burst)); // burst times should be positive

		// generate start time
		fprintf(f, "%d  start %d prio %d\n", i, EXP(start), NORMAL(20, 5) % 40);

		// generate cpu and io bursts
		// may use a further macro here
		for (int j = burstnum; j; --j) {
			fprintf(f, "%d cpu %d\n", i, 10 + EXP(cpu));
			fprintf(f, "%d io %d\n",  i, 10 + EXP(io));
		}
		fprintf(f, "%d cpu %d\n", i, 10 + EXP(cpu));
		fprintf(f, "%d end\n", i);
	}
	fclose(f);
}
