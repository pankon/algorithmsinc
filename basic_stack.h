/*
 * basic stack
 */

#ifndef __BASIC_STACK_H__
#define __BASIC_STACK_H__

#ifdef DEBUG_STACK
#define DBG_STACK(item) item
#else
#define DBG_STACK(item) 
#endif 

typedef struct stack stack_t;

stack_t *StackCreate(void);
void StackDestroy(stack_t *this);

int StackPush(stack_t *this, void *data); /* return 1 on success */
void *StackPop(stack_t *this);

int StackIsEmpty(stack_t *this);


#endif /* __BASIC_STACK_H__ */