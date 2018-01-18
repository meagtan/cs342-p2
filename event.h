/*
 * CS 342 Project 2
 * Ata Deniz Aydin
 * 21502637
 *
 * Event queue dispatching next event that will trigger a scheduling decision.
 *
 * There are three main types of events that cause an interrupt and trigger a scheduling decision:
 * - Timer ticks, occurring independently of any processing event
 * - A process requesting to use CPU, whether newly created or after I/O burst
 * - Running process finishing CPU burst voluntarily
 * In order to store such events in an event queue and dispatch them by order of occurrence, we need to
 *  keep in mind that, while the time the next timer tick or process arrival will occur can be determined
 *  ahead of time, the time the running process will finish CPU burst may change due to a future event
 *  causing it to be preempted.
 * Hence, keeping all types of events in the same queue would require changing the priority of the third
 *  type of events if necessary.
 * The following struct and functions implement an abstract event queue that keeps these in mind.
 */

#include "specs.h"
#include "rbtree.h"

typedef struct {
	rbtree arrivals; // event queue for arrival events, storing each process ordered by arrival time
	timeunit finish; // the time the running process is expected to finish
	timeunit last; // time of last event that has occurred, to calculate next timer tick
} evtqueue;

// types of events
enum {TICK, ARRIVAL, FINISH};

// set finish to IDLE whenever running process leaves
// since timeunit is unsigned, this makes finish larger than any other time value
#define IDLE -1

void evt_init(evtqueue *);

// add process to event queue, indexed by time of arrival
void evt_addproc(evtqueue *, pcb *, timeunit);

// time of next event
timeunit evt_min(evtqueue *);

// retrieve next event, pass time of occurrence and arriving process (if any) in parameter,
// return type of event in return value
int evt_pop(evtqueue *, timeunit *, pcb **);

// set finish time of running process
void evt_setfinish(evtqueue *, pcb *, timeunit);

// whether no process will arrive or finish execution
int evt_empty(evtqueue *);

void evt_free(evtqueue *);
