#include <stdlib.h>

#include "rbtree.h"

// red-black tree to implement later, will just implement ordered linked list now
/*

typedef struct _node {
	pcb *process;
	struct _node *child[2], *parent;
	int color;
	int key;
} node;

struct _rbtree {
	node *root, *min;
};

*/

// linked list

typedef struct _node {
	pcb *process;
	struct _node *next, *prev;
	int key;
} node;

struct _rbtree {
	node *root;
};

void rbtree_init(rbtree *tree)
{
	tree->root = NULL;
}

void rbtree_add(rbtree *tree, pcb *p, int key)
{
	// create node
	node *n = malloc(sizeof(node));
	n->process = p;
	n->next = n->prev = NULL;
	n->key = key;

	// add it to list
	if (tree->root == NULL) {
		tree->root = n;
	} else {
		// traverse list until reach end or larger key
		node *temp = tree->root;
		while (temp->next != NULL && temp->next->key < key)
			temp = temp->next;
		temp->next->prev = n;
		n->next = temp->next;
		n->prev = temp;
		temp->next = n;
	}
}

pcb *rbtree_get(rbtree *tree, int pid)
{
	node *temp = tree->root;
	while (temp != NULL && temp->process->pid != pid)
		temp = temp->next;
	return temp;
}

// retrieve and remove minimum element
pcb *rbtree_pop(rbtree *tree)
{
	if (tree->root == NULL)
		return NULL;
	pcb *res = tree->root->process;
	free(tree->root);
	return res;
}

// retrieve minimum element
pcb *rbtree_min(rbtree *tree)
{
	if (tree->root == NULL)
		return NULL;
	return tree->root->process;
}

// in-order successor
pcb *rbtree_succ(rbtree *tree, pcb *proc)
{
	node *temp = tree->root;
	while (temp != NULL && temp->process != proc)
		temp = temp->next;
	return temp ? temp->next : NULL;
}

void rbtree_free(rbtree *tree)
{
	node *temp = tree->root, temp1;
	while (temp != NULL) {
		temp1 = temp->next;
		free(temp);
		temp = temp1;
	}
}


