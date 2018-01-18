/*
 * CS 342 Project 2
 * Ata Deniz Aydin
 * 21502637
 *
 * Red-black tree storing pcbs indexed by an unsigned long long key.
 * Currently implemented as simple ordered linked lists
 */

#ifndef __RBTREE_H
#define __RBTREE_H

#include "pcb.h"
#include "specs.h"

typedef timeunit keyunit;

// ordered linked list implementation
typedef struct _node {
	pcb *process;
	struct _node *next, *prev;
	keyunit key; // may be both unsigned int or time unit
} node;

typedef struct {
	node *root;
	node *min;
} rbtree;

void rbtree_init(rbtree *);

int rbtree_empty(rbtree *);

void rbtree_add(rbtree *, pcb *, keyunit key);

pcb *rbtree_get(rbtree *, int pid);

// retrieve and remove minimum element
pcb *rbtree_pop(rbtree *);

// retrieve minimum element
node *rbtree_min(rbtree *);

// in-order successor
node *rbtree_succ(rbtree *, node *);

// set freepcb to 1 in order to free pcbs as well as nodes
void rbtree_free(rbtree *, int freepcb);

#endif
