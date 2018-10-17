/*
 * Btree
 */

typedef struct btree_node
{
	struct btree_node *parent;

	int nChildren;
	struct btree_node *children;
	
	int nElements;
	size_t eSize;
	void *array;
} btree_node_t;

typedef struct btree
{
	int (*alloc)(size_t size, void *params);
	void (*free)(void *ptr, void *params);
	int (*compare)(void *a, void *b); /* 1 greater, -1 less than, 0 equal */

	btree_node_t *root;
} btree_t;