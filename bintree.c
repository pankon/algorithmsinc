/*
 * binary tree functions 
 */

#include <stdio.h> /* fprintf, NULL */
#include <stdlib.h>	/* malloc, free */

#include "basic_stack.h"
#include "basic_queue.h"
#include "bintree.h"

bintree_t *BinTreeCreate(void)
{
	bintree_t *tree = NULL;

	if (NULL == (tree = malloc(sizeof(bintree_t))))
	{
		DBG_BINTREE(fprintf(stderr, "[BinTreeCreate] : error in malloc\n"));
		return (NULL);
	}

	tree->data = NULL;
	tree->left = NULL;
	tree->right = NULL;

	return tree;
}

static int BinTreeForEachPreOrder(bintree_t *root, int (*func)(bintree_t *tree, void *params), void *params)
{
	stack_t *stack = NULL;
	bintree_t *tree = NULL;
	int code = 0; 

	if (NULL == root)
	{
		return 1;
	}

	if (NULL == (stack = StackCreate()))
	{
		DBG_BINTREE(fprintf(stderr, "[BinTreeForEachPreOrder] : error in [StackCreate]\n"));
		return 1;
	}

	if (1 != StackPush(stack, (void *)root))
	{
		DBG_BINTREE(fprintf(stderr, "[BinTreeForEachPreOrder] : error in [StackPush]\n"));
		return 1;
	}

	DBG_STACK(printf("\n[StackIsEmpty] : %d\n", StackIsEmpty(stack)));
	while (0 == StackIsEmpty(stack) && 0 == code)
	{
		tree = (bintree_t *)StackPop(stack);
		DBG_BINTREE(printf("[BinTreeForEachPreOrder] : %p\n", tree));
		code = func(tree, params);

		if (NULL != tree->right)
		{
			StackPush(stack, (void *)tree->right);
		}

		if (NULL != tree->left)
		{
			StackPush(stack, (void *)tree->left);
		}
		DBG_STACK(printf("[StackIsEmpty] : %d\n", StackIsEmpty(stack)));
	}

	return code;
}

static int BinTreeForEachInOrder(bintree_t *root, int (*func)(bintree_t *tree, void *params), void *params)
{
	queue_t *queue = NULL;
	bintree_t *tree = NULL;
	int code = 0; 

	if (NULL == root)
	{
		return 1;
	}

	if (NULL == (queue = QueueCreate()))
	{
		DBG_BINTREE(fprintf(stderr, "[BinTreeForEachInOrder] : error in [QueueCreate]\n"));
		return 1;
	}

	if (1 != QueuePut(queue, (void *)root))
	{
		DBG_BINTREE(fprintf(stderr, "[BinTreeForEachInOrder] : error in [QueuePut]\n"));
		return 1;
	}

	DBG_QUEUE(printf("\n[QueueIsEmpty] : %d\n", QueueIsEmpty(queue)));
	while (0 == QueueIsEmpty(queue) && 0 == code)
	{
		code = func(tree, params);

		tree = (bintree_t *)QueueGet(queue);
		DBG_BINTREE(printf("[BinTreeForEachInOrder] : %p\n", tree));
		if (NULL != tree->right)
		{
			QueuePut(queue, (void *)tree->right);
		}

		if (NULL != tree->left)
		{
			QueuePut(queue, (void *)tree->left);
		}
		DBG_STACK(printf("[QueueIsEmpty] : %d\n", QueueIsEmpty(queue)));
	}

	return code;
}

static int BinTreeForEachPostOrder(bintree_t *root, int (*func)(bintree_t *tree, void *params), void *params)
{
	return -1;
}

static int BinTreeForEachLevelOrder(bintree_t *root, int (*func)(bintree_t *tree, void *params), void *params)
{
	/* not quite right, use queue rather than stack */
	stack_t *stack = NULL;
	bintree_t *tree = NULL;
	int code = 0; 

	if (NULL == root)
	{
		return 1;
	}

	if (NULL == (stack = StackCreate()))
	{
		DBG_BINTREE(fprintf(stderr, "[BinTreeForEachLevelOrder] : error in [StackCreate]\n"));
		return 1;
	}

	if (1 != StackPush(stack, (void *)root))
	{
		DBG_BINTREE(fprintf(stderr, "[BinTreeForEachLevelOrder] : error in [StackPush]\n"));
		return 1;
	}

	DBG_STACK(printf("\n[StackIsEmpty] : %d\n", StackIsEmpty(stack)));

	code = func(root, params);
	while (0 == StackIsEmpty(stack) && 0 == code)
	{
		tree = (bintree_t *)StackPop(stack);
		DBG_BINTREE(printf("[BinTreeForEachLevelOrder] : %p\n", tree));

		if (NULL != tree->right)
		{
			StackPush(stack, (void *)tree->right);
			code &= func(tree->right, params);
		}

		if (NULL != tree->left)
		{
			StackPush(stack, (void *)tree->left);
			code &= func(tree->left, params);
		}

		DBG_STACK(printf("[StackIsEmpty] : %d\n", StackIsEmpty(stack)));
	}

	return code;
}

int BinTreeForEach(bintree_t *root, TREE_ORDER order, int (*func)(bintree_t *tree, void *params), void *params)
{
	int code = 0;

	switch (order)
	{
		case PREORDER:
			code = BinTreeForEachPreOrder(root, func, params);
			break;
		case INORDER:
			code = BinTreeForEachInOrder(root, func, params);
			break;
		case POSTORDER:
			code = BinTreeForEachPostOrder(root, func, params);
			break;
		case LEVELORDER:
			code = BinTreeForEachLevelOrder(root, func, params);
			break;
	}

	return code;
}

void *BinTreeFree(bintree_t *tree)
{
	void *data = NULL;

	if (NULL != tree)
	{
		data = tree->data;
		tree->left = NULL;
		tree->right = NULL;
		free(tree);
		tree = NULL;
	}

	return data;
}

/* probably not so good to do it this way */
static int BinTreeDestroySingleTree(bintree_t *tree, void *null_params)
{
	free(tree);
	tree = NULL;

	return 0;
}

void BinTreeDestroy(bintree_t *tree)
{
	BinTreeForEach(tree, POSTORDER, BinTreeDestroySingleTree, NULL);
}
