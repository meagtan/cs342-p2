// Scheduling parameters and other common definitions go here

#ifndef __SPECS_H
#define __SPECS_H

// #include <stdint.h>

#define GRANULARITY 10000000

#define LATENCY (20 * GRANULARITY)

// use 64-bit unsigned integers for time in nanoseconds, to avoid overflows
typedef unsigned long long timeunit;

#endif
