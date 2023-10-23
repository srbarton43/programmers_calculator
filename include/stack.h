#ifndef __STACK_H

#include <stdlib.h>

typedef struct stack stack;

stack* new_stack();
/*
 * Check if stack is isEmpty
 *
 * parameters:
 *  stk => pointer to stack
 * returns:
 *  1 if empty
 *  0 if not
 */
int isEmpty(stack* stk);

/*
 * Pop from top of stack
 *
 * parameters:
 *  stk => pointer to stack
 *
 * returns:
 *  item from top of stack
 *  NULL if empty
 */
void* pop(stack* stk);

/*
 * Push to top of stack
 *
 * parameters:
 *  stk => stack pointer
 *  item => item to push
 * returns:
 *  nothing
 */
void push(stack* stk, void* item);

#endif // __STACK_H
