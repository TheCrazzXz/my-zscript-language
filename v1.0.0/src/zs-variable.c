#include "zs-variable.h"
#include <stdlib.h>
#include <string.h>
#include "expr-tree.h"
#include "zs-data.h"
#include "zs-error.h"
#include "expr-parser.h"
#include "args-parser.h"
#include "arithmetic.h"

size_t zs_alloc_var(zs_state* state, zs_var_type var_type, size_t var_size)
{
    size_t index;
    zs_var* allocated_data;

    index = state->nVars;

    ALLOCATION_FOR_NEW_ELEM_VECTOR(state->variables, state->nVars, sizeof(zs_var));
    
    allocated_data = &state->variables[index];

    allocated_data->data.size = var_size;
    allocated_data->data.type = var_type;

    return index;
}

ssize_t zs_get_variable_index(zs_state* state, char* varname)
{
    size_t i;
    for(i = 0 ; i < state->nVars ; i++)
    {
        if(strcmp(state->variables[i].name, varname) == 0)
        {
            return (ssize_t)i;
        }
    }
    return -1;
}

int variable_exists(zs_state* state, char* name)
{
    size_t i;

    for(i = 0 ; i < state->nVars ; i++)
    {
        if(strcmp(state->variables[i].name, name) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int zs_alter_variable(zs_state* state, size_t index, char* input_value, size_t lineno)
{
    zs_data data;
    
    data = zs_interpret_expression(state, zs_interpret_expression_input(input_value, lineno), ZS_TRUE);

    if(data.type == ZS_TYPE_ERROR || index + 1 > state->nVars)
    {
        zs_error("Couldn't alter variable");
        return 0;
    }

    if(data.type != state->variables[index].data.type)
    {
        zs_error("Wrong initializer");
        free(data.data);
        return 0;
    }   
    
    /*
        previous bad code that doesn't update size

        state->variables[index].data.data = data.data;

        other bad code after :

        free(state->variables[index].data.data);
    */
    clean_data_if_not_reference(&state->variables[index].data, state);
    state->variables[index].data = data;
    return 1;
}
int zs_operate_variable(zs_state* state, size_t index, char* input_value, zs_expr_type expr_type, size_t lineno)
{
    zs_data data;
    zs_var_type type;
    data = zs_interpret_expression(state, zs_interpret_expression_input(input_value, lineno), ZS_TRUE);

    if(data.type == ZS_TYPE_ERROR || index + 1 > state->nVars)
    {
        zs_error("Couldn't create variable");
        return 0;
    }

    if(data.type != state->variables[index].data.type)
    {
        zs_error("Wrong initializer");
        free(data.data);
        return 0;
    } 

    if(state->variables[index].data.type != ZS_TYPE_INT && state->variables[index].data.type != ZS_TYPE_FLOAT)
    {
        zs_error("trying to execute operation in something not a number");
        return 0;
    }

    state->variables[index].data.data = zs_operation(state->variables[index].data.data, data.data, state->variables[index].data.type, expr_type, state->variables[index].data.data);
    return 1;
}

int zs_create_variable(zs_state* state, char* input_name, char* input_value, zs_var_type var_type, size_t lineno)
{
    size_t index;
    size_t var_size;
    zs_data data;

    if(variable_exists(state, input_name))
    {
        zs_error("Variable %s already exists", input_name);
        return 0;
    }

    data = zs_interpret_expression(state, zs_interpret_expression_input(input_value, lineno), ZS_TRUE);

    if(data.type != var_type)
    {
        free(data.data);
        zs_error("Wrong initializer");
        return 0;
    }   
    if(data.type == ZS_TYPE_ERROR)
    {
        free(data.data);
        zs_error("Couldn't create variable");
        return 0;
    }
    /*
        previous bad code that causes malloc(): corrupted top size
        if(var_type == ZS_TYPE_STRING)
        {
            var_size = data.size + 1;
        }
        else if(var_type == ZS_TYPE_INT)
        {
            var_size = sizeof(int);
        }
        else if(var_type == ZS_TYPE_FLOAT)
        {
            var_size = sizeof(float);
        }
    */
    var_size = data.size;
    
    index = zs_alloc_var(state, var_type, var_size);

    state->variables[index].name = malloc(strlen(input_name) + 1);
    strcpy(state->variables[index].name, input_name);

    state->variables[index].data = data;
    
    return 1;
}

char* zs_get_variable_as_string(zs_state* state, size_t index, zs_bool as_expression)
{
    zs_var* var;

    var = &state->variables[index];

    return zs_get_data_as_string(&var->data, as_expression);
}