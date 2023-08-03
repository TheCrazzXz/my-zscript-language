#include "ptrstack.h"
#include "memutils.h"
#include <string.h>

ptrstack create_ptrstack()
{
    ptrstack ps;

    memset(&ps, 0, sizeof(ps));
    ps.top = -1;

    return ps;
}

void ptrstack_push(ptrstack* ps, void* ptr)
{
    ALLOCATION_FOR_NEW_ELEM_VECTOR(ps->mem, ps->size, sizeof(void*));
    ps->mem[++ps->top] = ptr;
}

void* ptrstack_peek(ptrstack* ps)
{
    return ps->mem[ps->top];
}

void* ptrstack_pop(ptrstack* ps)
{
    return ps->mem[ps->top--];
}

void free_ptrstack(ptrstack* ps)
{
    free(ps->mem);
}