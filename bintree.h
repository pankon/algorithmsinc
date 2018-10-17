/*
 * binary tree functions 
 */

#ifndef __BINTREE_H__
#define __BINTREE_H__

#ifdef DEBUG_BINTREE
#define DBG_BINTREE(item) item
#else
#define DBG_BINTREE(item) 
#endif 

typedef struct bintree
{
	void *data;
	struct bintree *left;
	struct bintree *right;
} bintree_t;

typedef enum 
{
	PREORDER,
	INORDER,
	POSTORDER,
	LEVELORDER,
} TREE_ORDER;

bintree_t *BinTreeCreate(void);

void *BinTreeFree(bintree_t *tree);

void BinTreeDestroy(bintree_t *tree);

/* any return of not 0 stops iteration, returns last code on failure and 0 on success */
int BinTreeForEach(bintree_t *root, TREE_ORDER order, int (*func)(bintree_t *tree, void *params), void *params);

#endif /* __BINTREE_H__ */