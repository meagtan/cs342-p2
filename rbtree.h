#include "pcb.h"

typedef struct _rbtree rbtree;

void rbtree_init(rbtree *);

void rbtree_add(rbtree *, pcb *, int key);

pcb *rbtree_get(rbtree *, int pid);

// retrieve and remove minimum element
pcb *rbtree_pop(rbtree *);

// retrieve minimum element
pcb *rbtree_min(rbtree *);

// in-order successor
pcb *rbtree_succ(rbtree *, pcb *);

void rbtree_free(rbtree *);
