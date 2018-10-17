/*
 * parse_tree - from page 41, Algorithms in C
 */

#include <stdio.h> /* fprintf, printf, scanf */

#include "basic_stack.h"
#include "bintree.h"

#ifdef DEBUG_MAIN
#define DBG_MAIN(item) item
#else 
#define DBG_MAIN(item) 
#endif

int PrintTree(bintree_t *tree, void *params)
{
	if (NULL != tree)
	{
		if (NULL != tree->data)
		{
			DBG_MAIN(printf("%s<%p %4c>", (char *)params, (void *)tree,
					(char)(int)(unsigned long)tree->data));
#ifndef DEBUG_MAIN
			printf("%c ", (char)(int)(unsigned long)tree->data);
#endif
		}
	}

	return 0;
}

int main(void)
{
	stack_t *stack = NULL;
	bintree_t *tree = NULL;
	bintree_t *root = NULL;
	char c = 0;

	if (NULL == (stack = StackCreate()))
	{
		return 1;
	}


	while ('\n' != scanf("%ls", &c))
	{
		if (NULL == (tree = BinTreeCreate()))
		{
			fprintf(stderr, "[main] : error in [BinTreeCreate] -> could not parse\n");
			return 1;
		}

		if (NULL == root)
		{
			root = tree;
		}

		tree->data = (void *)(size_t)c;
		if ('+' == c || '*' == c)
		{
			tree->right = (bintree_t *)StackPop(stack);
			DBG_MAIN(PrintTree(tree->right, "popped");
			putchar('\n'));
			tree->left = (bintree_t *)StackPop(stack);
			DBG_MAIN(PrintTree(tree->left, "popped");
			putchar('\n'));
		}

		DBG_MAIN(PrintTree(tree, "pushed"));
		if (!StackPush(stack, (void *)tree))
		{
			fprintf(stderr, "[main] : error in [StackPush] -> could not parse\n");
			return 1;
		}

		BinTreeForEach(tree, LEVELORDER, PrintTree, "");
		putchar('\n');
	}

	return 0;
}