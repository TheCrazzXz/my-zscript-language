#include "io-utils.h"
#include <stdio.h>

size_t dynamic_input(char** buf, FILE* fp)
{
    char* buffer;
    size_t len = 0;
    char c = 0x00;

        

    while(c != '\n')
    {
        
        c = fgetc(fp);
        
        //printf("len=%lu\n", len);
        if(len == 0)
        {
            buffer = malloc(1);
            //printf("buf (malloc) : %p\n", buffer);
        }
        else
        {
            buffer = realloc(buffer, len+1);
            //printf("buf (realloc) : %p\n", buffer);
        }

        if(c == '\n' || feof(fp))
        {
            buffer[len] = '\x00';
            //printf("buf (realloc) : %p\n", buffer);
            *buf = buffer;
            break;
        }

        //printf("realloc\n");
        buffer[len] = c;
        len++;
        //printf("ok\n");
        
    }
    return len;
}