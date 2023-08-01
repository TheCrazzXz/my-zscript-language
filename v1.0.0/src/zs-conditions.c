#include "zs-conditions.h"
#include "zs-loop.h"
#include "expr-tree.h"
#include "zs-error.h"
#include <string.h>

#define zs_test_condition(cs, cond) { \
    if(cond) \
    { \
        cs.value = ZS_TRUE; \
    } \
    else \
    { \
        cs.value = ZS_FALSE; \
    } \
}

stack_data get_cond_struct_stack_data(zs_conditionnal_structure cs)
{
    stack_data data;

    data.size = sizeof(zs_conditionnal_structure);
    data.data = malloc(data.size);
    memcpy(data.data, &cs, data.size);

    return data;
}

int zs_new_conditionnal_structure(zs_state* state, bt_node* expression_node)
{
    zs_data data;
    zs_conditionnal_structure cs;

    data = zs_interpret_expression(state, expression_node, ZS_TRUE);
    
    if(data.type == ZS_TYPE_ERROR)
    {
        zs_error("could not interpret expression for if");
        return 1;
    }

    zs_test_condition(cs, *(int*)data.data);
    cs.type = ZS_CONDTIONNAL_STRUCTURE_IF;
    cs.shadow = ZS_FALSE;

    clean_data_if_not_reference(&data, state);

    stack_push(&state->cond_structs, get_cond_struct_stack_data(cs));

    return 1;
}

zs_bool should_condition_code_be_executed(zs_state* state)
{
    return (((*stack_top(state->cond_structs, zs_conditionnal_structure)).value == 0 
        && (*stack_top(state->cond_structs, zs_conditionnal_structure)).type == ZS_CONDTIONNAL_STRUCTURE_ELSE) 
        || ((*stack_top(state->cond_structs, zs_conditionnal_structure)).value == 1 
        && (*stack_top(state->cond_structs, zs_conditionnal_structure)).type == ZS_CONDTIONNAL_STRUCTURE_IF));
}

int zs_check_shadow_if(zs_state* state)
{
    zs_conditionnal_structure cs;
    /* if there's another conditionnal structure */
    if(!is_stack_empty(state->cond_structs))
    {
        /* if this if block should not be interpreted, shadow it */
        if(!should_condition_code_be_executed(state))
        {
            cs.shadow = ZS_TRUE;
            cs.type = ZS_CONDTIONNAL_STRUCTURE_IF;
            stack_push(&state->cond_structs, get_cond_struct_stack_data(cs));

            return 1;
        }
    }
    return 0;
}

int zs_check_condition_code_to_be_executed(zs_state* state)
{
    /* if in a condition block */
    if(!is_stack_empty(state->cond_structs))
    {
        /* if code should not be executed because the condition or is shadowed, doesndon't interpret this line*/
        if(should_condition_code_be_executed(state) && !(*stack_top(state->cond_structs, zs_conditionnal_structure)).shadow)
        {
            //printf("CODE NOT EXECUTED DUE TO IF\n");
            return 1; 
        }
        return 0;
    }
    return 1;
}
int zs_new_else_statement(zs_state* state)
{
    /* if we're not in a cond structure or if current type is not if, error */
    if(is_stack_empty(state->cond_structs) || (*stack_top(state->cond_structs, zs_conditionnal_structure)).type != ZS_CONDTIONNAL_STRUCTURE_IF)
    {
        zs_error("else not in an if statement structure");
        return 0;
    }
    /* set current type to else */
    (*stack_top(state->cond_structs, zs_conditionnal_structure)).type = ZS_CONDTIONNAL_STRUCTURE_ELSE;
    return 1;
}

int zs_check_end_conditionnal_statement(zs_state* state)
{
    /* error if there's no current conditionnal structure */
    if(is_stack_empty(state->cond_structs))
    {
        zs_error("endif not in an if statement structure");
        return 0;
    }
    /* pop the current conditionnal structure */
    stack_pop(&state->cond_structs);
    return 1;
}