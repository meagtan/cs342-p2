#include <stdio.h> // testing
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



// linked list

struct _node {
	pcb *process;
	struct _node *next, *prev;
	int key;
};


struct _rbtree {
	node *root;
};
*/

void rbtree_init(rbtree *tree)
{
	tree->root = calloc(1, sizeof(node)); // dummy head node
	tree->root->process = calloc(1, sizeof(pcb));
	tree->min = NULL;
}

int rbtree_empty(rbtree *tree)
{
	return tree->min == NULL;
}

void rbtree_add(rbtree *tree, pcb *p, keyunit key)
{
	// create node
	node *n = malloc(sizeof(node));
	n->process = p;
	n->next = n->prev = NULL;
	n->key = key;

	// add n to list at correct place
	/*
	if (tree->root == NULL || tree->root->key < key) {
		if (tree->root) {
			tree->root->prev = n;
			n->next = tree->root;
		}
		tree->root = n;
	} else {
	*/
	// traverse list until reach end or larger key
	node *temp = tree->root;
	while (temp->next != NULL && temp->next->key <= key)
		temp = temp->next;
	if (temp->next) {
		temp->next->prev = n;
		n->next = temp->next;
	}
	temp->next = n;
	n->prev = temp;

	tree->min = tree->root->next;
}

pcb *rbtree_get(rbtree *tree, int pid)
{
	node *temp = tree->min;
	while (temp && temp->process->pid != pid)
		temp = temp->next;
	return temp ? temp->process : NULL;
}

// retrieve and remove minimum element
pcb *rbtree_pop(rbtree *tree)
{
	if (tree->min == NULL)
		return NULL;

	pcb *res = tree->min->process;
	if (tree->root->next = tree->min->next)
		tree->root->next->prev = tree->root;
	free(tree->min);
	tree->min = tree->root->next;
	return res;
}

// retrieve minimum element
node *rbtree_min(rbtree *tree)
{
	return tree->min;
}

// in-order successor
node *rbtree_succ(rbtree *tree, node *node)
{
	/*
	node *temp = tree->root;
	while (temp != NULL && temp->process != proc)
		temp = temp->next;
	return temp ? temp->next : NULL;
	*/
	return node->next;
}

void rbtree_free(rbtree *tree, int freepcb)
{
	node *temp = tree->root, *temp1;
	while (temp != NULL) {
		if (freepcb || temp == tree->root) // workaround
			pcb_free(temp->process);
		temp1 = temp->next;
		free(temp);
		temp = temp1;
	}
	 // testing
	// free(tree);
}


