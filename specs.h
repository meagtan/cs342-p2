/*
 * CS 342 Project 2
 * Ata Deniz Aydin
 * 21502637
 *
 * Scheduling parameters and other common definitions
 */

#ifndef __SPECS_H
#define __SPECS_H

// #include <stdint.h>

#define GRANULARITY 10000000

#define LATENCY (20 * GRANULARITY)

// use 64-bit unsigned integers for time in nanoseconds, to avoid overflows
typedef unsigned long long timeunit;

#endif
