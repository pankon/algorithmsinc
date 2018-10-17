/*
 * basic queue
 */

#ifndef __BASIC_QUEUE_H__
#define __BASIC_QUEUE_H__

#ifdef DEBUG_QUEUE
#define DBG_QUEUE(item) item
#else
#define DBG_QUEUE(item) 
#endif

typedef struct queue_node
{
	void *data;
	struct queue_node *next;
	struct queue_node *prev;
} queue_node_t;

typedef struct queue
{
	queue_node_t *head;
	queue_node_t *tail;
} queue_t;

queue_t *QueueCreate(void);
void QueueDestroy(queue_t *queue);

int QueuePut(queue_t *queue, void *data);
void *QueueGet(queue_t *queue);

int QueueIsEmpty(queue_t *queue);

#endif /* __BASIC_QUEUE_H__ */