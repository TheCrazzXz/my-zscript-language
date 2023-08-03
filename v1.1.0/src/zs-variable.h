#ifndef __ZS_VARIABLE_H__
#define __ZS_VARIABLE_H__

#include "zs-data.h"
#include "zs-expression.h"
#include <stddef.h>
#include <sys/types.h>

typedef struct zs_variable_t
{
    zs_data data;
    char* name;
} zs_var;

size_t zs_alloc_var(zs_state* state, zs_var_type var_type, size_t var_size);

ssize_t zs_get_variable_index(zs_state* state, char* varname);

int variable_exists(zs_state* state, char* name);

int zs_alter_variable(zs_state* state, size_t index, char* input_value, size_t lineno);

int zs_operate_variable(zs_state* state, size_t index, char* input_value, zs_expr_type expr_type, size_t lineno);

int zs_create_variable(zs_state* state, char* input_name, char* input_value, zs_var_type var_type, size_t lineno);

char* zs_get_variable_as_string(zs_state* state, size_t index, zs_bool as_expression);

#endif