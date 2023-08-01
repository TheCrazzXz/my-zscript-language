#ifndef __ARGS_PARSER_H__
#define __ARGS_PARSER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory/memutils.h"

size_t get_file_size(FILE* file);

/* strdup until a character */


void print_command_args(char** words, size_t num);

char** parse_command_args(char* buffer, size_t* num);

void parse_command_args_cleanup(char** args, size_t num);


#endif