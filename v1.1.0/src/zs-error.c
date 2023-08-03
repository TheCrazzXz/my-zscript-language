#include "zs-error.h"

void zs_error(const char* format, ...)
{
    va_list    args;
    va_start(args, format);
    
    fprintf(stderr, "ZSCRIPT ERROR : [");
    vfprintf(stderr, format, args);
    fprintf(stderr, "]\n");

    va_end(args);
}