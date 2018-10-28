/*
 * Btree 2-3-4 tree
 */

typedef struct btree_node
{
	struct btree_node *parent;

	int nChildren;
	struct btree_node *children;

	void *data;
} btree_node_t;

typedef struct btree
{
	int (*alloc)(size_t size, void *params);
	void (*free)(void *ptr, void *params);
	int (*compare)(void *a, void *b); /* 1 greater, -1 less than, 0 equal */

	btree_node_t *root;
} btree_t;

