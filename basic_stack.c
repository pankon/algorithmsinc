/*
 * Basic stack
 */

#include <stdio.h> /* NULL, fprintf */
#include <stdlib.h> /* malloc, free */
#include "basic_stack.h"

struct stack
{
	struct stack *next;
	void *data;
};

stack_t *StackCreate(void)
{
	stack_t *this = NULL;
	if (NULL == (this = malloc(sizeof(stack_t))))
	{
		DBG_STACK(printf(stderr, "[StackCreate] : Error in malloc\n"));
		return (NULL);
	}

	this->next = NULL;
	this->data = NULL;

	return this;
}

void StackDestroy(stack_t *this)
{
	stack_t *current = this;
	
	while (NULL != current)
	{
		current = StackPop(this);
		free(current);
		current = NULL;
	}

	free(this);
}

int StackPush(stack_t *this, void *data)
{
	stack_t *next = NULL;
	if (NULL == (next = StackCreate()))
	{
		return 0;
	}

	next->next = this->next;
	next->data = data;
	this->next = next;

	return 1;
}

void *StackPop(stack_t *this)
{
	void *data = NULL;
	stack_t *next = NULL;

	if (NULL != this)
	{
		DBG_STACK(printf("[StackPop: this = %p\n", this));
		if (NULL != this->next)
		{
			next = this->next;
			data = next->data;
			DBG_STACK(printf("[StackPop: this->next->data = %p\n", data));
			this->next = next->next;
		}
	}
	
	return data;
}

int StackIsEmpty(stack_t *this)
{
	if (NULL == this)
	{
		return -1;
	}

	return (NULL == this->next);
}