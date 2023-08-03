#include "zs-file.h"
#include "memory/memutils.h"
#include "zs-data.h"
#include "zs-error.h"

zs_file zs_openfile(char* name, char* mode, zs_bool* error)
{
    zs_file file;
    *error = ZS_FALSE;

    if(mode[0] == 'w')
    {
        file.mode = ZS_FILE_MODE_WRITE;
    }
    else if(mode[0] == 'a')
    {
        file.mode = ZS_FILE_MODE_WRITE;
    }
    else if(mode[0] == 'r')
    {
        file.mode = ZS_FILE_MODE_READ;
    }
    else
    {
        zs_error("unknown file mode : %s", mode);
        *error = ZS_TRUE;
        return file;
    }

    file.fp = fopen(name, mode);
    
    
    if(file.fp == NULL)
    {
        *error = ZS_TRUE;
        return file;
    }

    file.name = alloc_and_copy_string(name);
    return file;
}

void zs_closefile(zs_file* file)
{
    if(file->fp != NULL)
    {
        fclose(file->fp);
    }
    file->fp = NULL;
    
}