#ifndef __RBTREE_H
#define __RBTREE_H

#include "pcb.h"

// ordered linked list implementation
typedef struct _node {
	pcb *process;
	struct _node *next, *prev;
	int key;
} node;

typedef struct {
	node *root;
	node *min;
} rbtree;

void rbtree_init(rbtree *);

int rbtree_empty(rbtree *);

void rbtree_add(rbtree *, pcb *, int key);

pcb *rbtree_get(rbtree *, int pid);

// retrieve and remove minimum element
pcb *rbtree_pop(rbtree *);

// retrieve minimum element
node *rbtree_min(rbtree *);

// in-order successor
node *rbtree_succ(rbtree *, node *);

void rbtree_free(rbtree *, int freepcb);

#endif
