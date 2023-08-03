#ifndef __ZS_FUNCTIONS_H__
#define __ZS_FUNCTIONS_H__

#include "zs-data.h"
#include <stdlib.h>
#include <stdio.h>

#define zs_readint(ptr) scanf("%d", (int*)ptr); fgetc(stdin);
#define zs_readfloat(ptr) scanf("%f", (float*)ptr); fgetc(stdin);
#define zs_readstring(ptr) dynamic_input(ptr, stdin)
#define randint(min, max) ((rand() % (max + 1 - min)) + min)

int zs_print(zs_state* state, char** args, size_t nArgs, zs_bool newline, size_t lineno);
int zs_builin_function_execute(zs_state*, char* name, zs_data* data, zs_data* arguments, size_t nArgs);

#endif