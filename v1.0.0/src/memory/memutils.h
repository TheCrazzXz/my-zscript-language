#ifndef __MEMUTILS_H__
#define __MEMUTILS_H__

#define ALLOCATION_FOR_NEW_ELEM_VECTOR(data, len, size) {\
    len++;\
    if(len == 1)\
        data = malloc(size);\
    else\
        data = realloc(data, size * len);\
}

char* alloc_and_copy_string(char* str);

#endif