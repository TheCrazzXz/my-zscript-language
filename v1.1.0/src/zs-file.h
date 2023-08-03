#ifndef __ZS_FILE_H__
#define __ZS_FILE_H__

#include <stdio.h>
#include "zs-data.h"

typedef enum __zs_file_mode_t
{
    ZS_FILE_MODE_READ,
    ZS_FILE_MODE_WRITE
} zs_file_mode;

typedef struct __zs_file_t
{
    char* name;
    FILE* fp;
    zs_file_mode mode;
    
} zs_file;

zs_file zs_openfile(char* name, char* mode, zs_bool* error);
void zs_closefile(zs_file* file);

#endif