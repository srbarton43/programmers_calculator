
#include <stdlib.h>

#include "../include/stack.h"

#define MAX_HEIGHT 100

typedef struct stack {
  void* stack[MAX_HEIGHT];
  int top;
} stack;

stack* new_stack() {
  stack* new_stk = malloc(sizeof(stack));

  return new_stk;
}
int isEmpty(stack* stk) {
  if (stk == NULL) return 1;

  return stk->top == 0;
}

void* pop (stack* stk) {
  if (isEmpty(stk)) return NULL;
  return stk->stack[stk->top--];
}

void push (stack* stk, void* item) {
  if (stk->top == MAX_HEIGHT) return;
  stk->top++;
  stk->stack[stk->top] = item;
}
