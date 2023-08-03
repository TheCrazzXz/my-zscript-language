#include "zs-function.h"
#include "memory/memutils.h"
#include "memory/stack.h"
#include "zs-error.h"
#include <stdio.h>
#include <string.h>

stack_data get_func_call_struct_stack_data(zs_function_call func_call)
{
    stack_data data;

    data.size = sizeof(zs_function_call);
    data.data = malloc(data.size);
    memcpy(data.data, &func_call, data.size);

    return data;
}

void zs_define_function_here(zs_state* state, char* name, char** parameters_expr, size_t nParams)
{
    zs_function func;
    size_t i;
    size_t j;
    func.name = alloc_and_copy_string(name);
    func.start_off = ftell(state->fp);

    ALLOCATION_FOR_NEW_ELEM_VECTOR(state->functions, state->nFuncs, sizeof(zs_function));
    state->functions[state->nFuncs - 1] = func;
    state->defining_function = ZS_TRUE;

    state->functions[state->nFuncs - 1].nParemters = nParams;

    j = 0;

    for(i = 0 ; i < nParams ; i++)
    {
        ALLOCATION_FOR_NEW_ELEM_VECTOR(state->functions[state->nFuncs - 1].parameters, j, sizeof(char*));
        state->functions[state->nFuncs - 1].parameters[i] = malloc(strlen(parameters_expr[i]) + 1);
        strcpy(state->functions[state->nFuncs - 1].parameters[i], parameters_expr[i]);
    }
}

ssize_t zs_function_parameter_index(zs_function* function, char* name)
{
    size_t i;
    for(i = 0 ; i < function->nParemters ; i++)
    {
        if(strcmp(function->parameters[i], name) == 0)
        {
            return (ssize_t)i;
        }
    }
    return -1;
}

zs_endfuncdef_state zs_end_function_define(zs_state* state)
{
    if(state->defining_function)
    {
        state->defining_function = ZS_FALSE;
        return ZS_ENDFUNCDEF_STATE_DEFINED;
    }
    else if(!is_stack_empty(state->call_stack))
    {
        fseek(state->fp, (*stack_top(state->call_stack, zs_function_call)).ret_from_function_off, SEEK_SET);
        stack_pop(&state->call_stack);
        return ZS_ENDFUNCDEF_STATE_END;
    }

    zs_error("endef not in a function define");
    return ZS_ENDFUNCDEF_STATE_ERROR;
}

ssize_t zs_get_function_index(zs_state* state, char* name)
{
    size_t i;
    for(i = 0 ; i < state->nFuncs ; i++)
    {
        if(strcmp(state->functions[i].name, name) == 0)
        {
            return (ssize_t)i;
        }
    }
    return -1;
}
zs_bool zs_function_exists(zs_state* state, char* name)
{
    size_t i;

    for(i = 0 ; i < state->nFuncs ; i++)
    {
        if(strcmp(state->functions[i].name, name) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int zs_call_function(zs_state* state, size_t function_index, zs_data* arguments, size_t nArgs)
{
    zs_function_call call;
    size_t i;
    call.ret_from_function_off = ftell(state->fp);
    call.function_index = function_index;
    fseek(state->fp, state->functions[function_index].start_off, SEEK_SET);

    if(nArgs != state->functions[function_index].nParemters)
    {
        zs_error("invalid number of arguments : (%lu given / %lu required)", nArgs, state->functions[function_index].nParemters);
        return 0;
    }
    //call.arguments = arguments;
    for(i = 0 ; i < nArgs ; )
    {
        ALLOCATION_FOR_NEW_ELEM_VECTOR(call.arguments, i, sizeof(zs_data));
        memcpy(&call.arguments[i - 1], &arguments[i - 1], sizeof(zs_data));
    }

    stack_push(&state->call_stack, get_func_call_struct_stack_data(call));
    return 1;
}