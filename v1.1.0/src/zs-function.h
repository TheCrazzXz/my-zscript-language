#ifndef __ZS_FUNCTION_H__
#define __ZS_FUNCTION_H__

#include "zs-data.h"

typedef struct zs_function_t
{
    char* name; /* name of the function */
    size_t start_off; /* offset where the function code block starts */
    char** parameters; /* name of parameters for the function */
    size_t nParemters; /* number of parameters for the function  */
} zs_function;

typedef struct zs_function_call_t
{
    size_t function_index ; /* index of the function to call */
    size_t ret_from_function_off; /* when called, return offset is saved */
    zs_data* arguments; /* arguments of the function */
} zs_function_call;

void zs_define_function_here(zs_state* state, char* name, char** parameters_expr, size_t nParams);
void zs_destroy_function(zs_function* func);
ssize_t zs_function_parameter_index(zs_function* function, char* name);

typedef enum __zs_endfuncdef_state_t
{
    ZS_ENDFUNCDEF_STATE_DEFINED,
    ZS_ENDFUNCDEF_STATE_END,
    ZS_ENDFUNCDEF_STATE_ERROR,
} zs_endfuncdef_state;

zs_endfuncdef_state zs_end_function_define(zs_state* state);
ssize_t zs_get_function_index(zs_state* state, char* name);
zs_bool zs_function_exists(zs_state* state, char* name);
int zs_call_function(zs_state* state, size_t function_index, zs_data* arguments, size_t nArgs);

#endif