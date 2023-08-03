#ifndef __PTRSTACK_H__
#define __PTRSTACK_H__

#include <stdlib.h>
#include "memutils.h"

typedef struct __pointer_stack_t
{
    void** mem;
    size_t size;
    ssize_t top;
} ptrstack;

ptrstack create_ptrstack();

void ptrstack_push(ptrstack* ps, void* ptr);
void* ptrstack_peek(ptrstack* ps);
void* ptrstack_pop(ptrstack* ps);
void free_ptrstack(ptrstack* ps);

#endif