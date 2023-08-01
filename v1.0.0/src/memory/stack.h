#ifndef __STACK_H__
#define __STACK_H__

#include <stdlib.h>
#include "memutils.h"

#define stack_elem(stack, index, type) (type*)(((stack).mem)[index].data)
#define stack_top(stack, type) stack_elem(stack, (stack).top, type)
#define is_stack_empty(stack) (stack.top < 0)
typedef struct _stack_data_t
{
    size_t size;
    void* data;
} stack_data;

typedef struct _stack_t
{
    stack_data* mem;
    size_t size;
    ssize_t top;
} stack;

stack create_stack();

void stack_push(stack* ps, stack_data data);
stack_data* stack_peek(stack* ps);
void stack_pop(stack* ps);
void free_stack(stack* ps);

#endif