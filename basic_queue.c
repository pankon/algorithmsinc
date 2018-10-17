/*
 * basic queue
 */

#include <stdio.h>  /* fprintf, NULL */
#include <stdlib.h> /* malloc, free */
#include "basic_queue.h"

#define DEADBEEF (void *)0xDEADBEEF

queue_t *QueueCreate(void)
{
	queue_t *queue = NULL;

	if (NULL == (queue = malloc(sizeof(queue_t))))
	{
		DBG_QUEUE(fprintf(stderr, "[QueueCreate] : error in malloc\n"));
		return (NULL);
	}

	queue->head = DEADBEEF;
	queue->tail = DEADBEEF;

	return (queue);
}

void QueueDestroy(queue_t *queue)
{
	if (NULL == queue)
	{
		return;
	}

 	while (0 == QueueIsEmpty(queue))
 	{
 		QueueGet(queue);
 	}

 	queue->head = NULL;
 	queue->tail = NULL;
 	free(queue);
 	queue = NULL;
}

int QueuePut(queue_t *queue, void *data)
{
	queue_node_t *current = NULL;

	if (NULL == (current = malloc(sizeof(queue_node_t))))
	{
		DBG_QUEUE(fprintf(stderr, "[QueuePut] : error in malloc\n"));
		return (0);
	}

	current->next = queue->head;
	current->prev = DEADBEEF;

	if (DEADBEEF == queue->tail)
	{
		queue->tail = current;
	}

	return (1);
}

void *QueueGet(queue_t *queue)
{
	void *data = NULL;
	queue_node_t *current = NULL;
	
	if (NULL != queue)
	{
		if (NULL != queue->tail)
		{
			current = queue->tail;
			queue->tail = current->prev;
			data = current->data;
			current->prev = NULL;
			free(current);
			current = NULL;
		}
	}

	return (data);
}

int QueueIsEmpty(queue_t *queue)
{
	if (NULL == queue)
	{
		return (-1);
	}

	return (queue->head == queue->tail);
}