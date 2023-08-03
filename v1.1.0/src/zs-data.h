#ifndef __ZS_DATA_H__
#define __ZS_DATA_H__

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "memory/stack.h"

typedef enum zs_bool_e
{
    ZS_FALSE,
    ZS_TRUE
} zs_bool;

typedef enum zs_variable_type_e
{
    ZS_TYPE_NONE,
    ZS_TYPE_INT,
    ZS_TYPE_STRING,
    ZS_TYPE_FLOAT,  
    ZS_TYPE_VECTOR,
    ZS_TYPE_FILE,
    ZS_TYPE_AUTO,
    ZS_TYPE_ERROR
} zs_var_type;


typedef struct zs_data_t
{
    size_t size;
    void* data;
    zs_var_type type;
    zs_bool isref;
} zs_data;

struct zs_variable_t;
struct zs_function_t;

typedef struct zs_state_t
{
    size_t nVars;
    struct zs_variable_t* variables;
    size_t nFuncs;
    struct zs_function_t* functions;
    zs_data function_return;
    zs_bool defining_function;
    stack cond_structs;
    stack loop_structs;
    stack call_stack;
    size_t ndel;
    size_t totaldelsize;
    FILE* fp;
} zs_state;

char* zs_get_data_as_string(zs_data* var, zs_bool as_expression);

zs_var_type zs_string_to_type(char* str);

int clean_data_if_not_reference(zs_data* data, zs_state* state);

size_t zs_data_type_to_size(zs_var_type type);
void zs_create_alloc_data(zs_data* data, zs_var_type type);
int zs_destroy_data(zs_data* data, zs_state* state);

#define zs_set_size_and_alloc(_data, _size) (_data).size = _size; \
    (_data).data = malloc((_data).size);

#endif