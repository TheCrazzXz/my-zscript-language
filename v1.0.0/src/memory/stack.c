#include "stack.h"
#include "memutils.h"
#include <string.h>


stack create_stack()
{
    stack s;

    memset(&s, 0, sizeof(s));
    s.top = -1;

    return s;
}

void stack_push(stack* s, stack_data data)
{
    ALLOCATION_FOR_NEW_ELEM_VECTOR(s->mem, s->size, sizeof(stack_data));
    s->mem[++s->top] = data;
}

stack_data* stack_peek(stack* s)
{
    return &s->mem[s->top];
}

void stack_pop(stack* s)
{
    free(s->mem[s->top--].data);
}

void free_stack(stack* s)
{
    free(s->mem);
}