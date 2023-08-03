#include "memutils.h"
#include <stdlib.h>
#include <string.h>

char* alloc_and_copy_string(char* str)
{
    char* ret;

    ret = malloc(strlen(str) + 1);
    strcpy(ret, str);

    return ret;
}