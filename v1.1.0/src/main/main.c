#include "../zscript-interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include "../config.h"
#include <string.h>

int __zs__dbg_level__ = 0;

int main(int argc, char** argv)
{
    FILE* fp;
    if(argc >= 2)
    {
        if(strcmp(argv[1], "-dbg") == 0)
        {
            fp = stdin;
            if(argc >= 3)
            {
                __zs__dbg_level__ = atoi(argv[2]);
            }
            else
            {
                __zs__dbg_level__ = 1;
            }
        }
        else
        {
            fp = fopen(argv[1], "r");
            if(fp == NULL)
            {
                fprintf(stderr, "Error : Couldn't open file %s to read\n", argv[1]);
                exit(EXIT_FAILURE);
            }
            
            if(argc >= 3)
            {
                if(strcmp(argv[2], "-dbg") == 0)
                {
                    if(argc >= 4)
                    {
                        __zs__dbg_level__ = atoi(argv[3]);
                    }
                    else
                    {
                        __zs__dbg_level__ = 1;
                    }
                }
                else
                {
                    fprintf(stderr, "Error : Usage : %s <program> [-dbg] [debug level]\n", argv[0]);
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                __zs__dbg_level__ = 0;
            }
        }
        
    }
    else
    {
        fp = stdin;
    }
    zs_intepret_code(fp);

    return 0;
}