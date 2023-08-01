#include "args-parser.h"
#include "strutils.h"
size_t get_file_size(FILE* file)
{
    size_t size;
    fseek(file, 0, SEEK_END); // seek to end of file
    size = ftell(file); // get current file pointer
    fseek(file, 0, SEEK_SET); // seek back to beginning of file
    // proceed with allocating memory and reading the file
    return size;
}





void print_command_args(char** words, size_t num)
{
    size_t i;

    for(i = 0 ; i < num ; i++)
    {
        printf("words[%lu] : [%s]\n", i, words[i]);
    }
}

char** parse_command_args(char* buffer, size_t* num)
{
    size_t len; 
    char** args;
    char* start;
    int quote;
    int escaping;
    start = buffer;
    *num = 0;
    len = 0;
    quote = 0;
    escaping = 0;
    
    for(;;)
    {
        switch(*buffer++)
        {
            case '"':
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
                
                break;
            case '\\':
                escaping = 1;
            case ' ':
            {
                if(!quote)
                {
                    ALLOCATION_FOR_NEW_ELEM_VECTOR(args, len, sizeof(char**));
                    args[len - 1] = strdupc(start, ' ', 1);
                    start = buffer;
                    (*num)++;
                }
                
                break;
            } 
            case '\0':
            {
                ALLOCATION_FOR_NEW_ELEM_VECTOR(args, len, sizeof(char**));
                args[len - 1] = strdup(start);
                (*num)++;
                return args;
                break;
            }
            default:
                break;
        }
    }
}

void parse_command_args_cleanup(char** args, size_t num)
{
    for(size_t i = 0 ; i < num ; i++)
    {
        free(args[i]);
    }
    free(args);
}
