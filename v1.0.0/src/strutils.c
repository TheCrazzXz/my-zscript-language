#include "strutils.h"
#include <string.h>
#include "memory/memutils.h"

ssize_t chridx(char* str, char c)
{
    char *e = strchr(str, c);
    if (e == NULL) {
        return -1;
    }
    return (ssize_t)(e - str);
}

ssize_t chridxn(char* str, char c, size_t n)
{
    size_t i;

    for(i = 0 ; i < n ; i++)
    {
        char *e = strchr(str, c);
        if (e == NULL) {
            return -1;
        }
        if(i == (n - 1))
        {
            return (ssize_t)(e - str);
        }
        
        str = e;
        str++;
        
    }
    return -1;
}

size_t chrocc(char* str, char c)
{
    size_t count;

    count = 0;

    while(*str)
    {
        if(*str == c)
        {
            count++;
        }
    }
    return count;
}
/* strdup until a character */
char* strdupc(char* str, char end, int enablequoting)
{
    size_t len = 0;
    char* alloc;
    int quote;
    int escaping;

    quote = 0;
    escaping = 0;

    for(;;)
    {
        ALLOCATION_FOR_NEW_ELEM_VECTOR(alloc, len, sizeof(char));

        if(*str == '"')
        {
            if(!escaping)
            {
                if(quote)
                {
                    quote = 0;
                }
                else
                {
                    quote = 1;
                }
            }
            else
            {
                escaping = 0;
            }
        }
        else if(*str == '\\')
        {
            escaping = 1;
        }

        if((*str == end || *str == '\0') && ((quote == 0) || !enablequoting))
        {
            alloc[len - 1] = '\0';
            return alloc;
        }
        alloc[len - 1] = *str;

        str++;
    }
}

ssize_t strarrstr(char** str_str, size_t len, size_t start_idx, char* find)
{
    size_t i;
    for(i = start_idx ; i < len ; i++)
    {
        if(strcmp(str_str[i], ";") == 0)
        {
            return (ssize_t)i;
        }
    }
    return -1;
}

