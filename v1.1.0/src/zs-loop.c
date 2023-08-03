#include "zs-loop.h"
#include "zs-data.h"
#include <string.h>
#include "expr-tree.h"
#include "zs-error.h"
#include "zs-conditions.h"
#include "zscript-interpreter.h"
stack_data get_loop_struct_stack_data(zs_loop_structure ls)
{
    stack_data data;

    data.size = sizeof(zs_loop_structure);
    data.data = malloc(data.size);
    memcpy(data.data, &ls, data.size);

    return data;
}

int zs_new_loop(zs_state* state, bt_node* expression_node, char* it_code)
{
    zs_loop_structure ls;
    zs_data data;
    bt_node* node_copy;

    ls.loop_expression = expression_node;
    ls.iteration_code = it_code;

    data = zs_interpret_expression_copy(state, ls.loop_expression, &node_copy);

    //printf("loop enter result : %d\n", *(int*)data.data);

    if(data.type == ZS_TYPE_ERROR)
    {
        zs_error("could not interpret expression for loop");
        return 0;
    }

    if(*(int*)data.data)
    {
        ls.value = ZS_TRUE;
        ls.loop_content_start_off = ftell(state->fp);
        ls.cracking_loop = ZS_FALSE;
    
        ls.shadow = ZS_FALSE;

        if(!is_stack_empty(state->loop_structs))
        {
            //printf("loop shadowed\n");
            /* if previous is cracking loop or shadowing or current conditionnal structure block is not executed or shadowing then shadow */
            ls.shadow = ((*stack_top(state->loop_structs, zs_loop_structure)).cracking_loop
            || (*stack_top(state->loop_structs, zs_loop_structure)).shadow);
        }
        if(!is_stack_empty(state->cond_structs))
        {
            if(ls.shadow != ZS_TRUE)
            {
                ls.shadow = !should_condition_code_be_executed(state)
                || (*stack_top(state->cond_structs, zs_conditionnal_structure)).shadow;
            }
        }
    }
    else
    {
        ls.value = ZS_FALSE;
        free_bt_node(ls.loop_expression);
    }
    stack_push(&state->loop_structs, get_loop_struct_stack_data(ls));

    clean_data_if_not_reference(&data, state);
    free_bt_node(node_copy);

    return 1;
}

zs_bool should_loop_code_be_executed(zs_state* state)
{
    return (((*stack_top(state->loop_structs, zs_loop_structure)).value == 1)
        && !(*stack_top(state->loop_structs, zs_loop_structure)).shadow
        && !(*stack_top(state->loop_structs, zs_loop_structure)).cracking_loop);
}

int zs_check_loop_code_to_be_executed(zs_state* state)
{
    /* if in a loop block*/
    if(!is_stack_empty(state->loop_structs))
    {
        /* if code shouldn't be execute because of the loop, don't interpret it  */
        if(should_loop_code_be_executed(state))
        {
            //printf("CODE NOT EXECUTED DUE TO LOOP\n");
            return 1;
        }
        return 0;
    }
    return 1;
    
}

void zs_end_current_loop(zs_state* state)
{
    if((*stack_top(state->loop_structs, zs_loop_structure)).value == ZS_TRUE)
    {
        destroy_expr_node_tree((*stack_top(state->loop_structs, zs_loop_structure)).loop_expression, state);
    }

    if((*stack_top(state->loop_structs, zs_loop_structure)).iteration_code != NULL)
    {
        free((*stack_top(state->loop_structs, zs_loop_structure)).iteration_code);
    }
    
    stack_pop(&state->loop_structs);
}

void zs_check_execute_iteration_code(zs_state* state)
{
    if((*stack_top(state->loop_structs, zs_loop_structure)).iteration_code != NULL)
    {
        zs_interpret_line((*stack_top(state->loop_structs, zs_loop_structure)).iteration_code, state, 0);
    }
}

zs_endloop_state zs_check_endloop(zs_state* state)
{
    zs_data data;
    bt_node* node_copy;

    /* error if there's no current loop structure */
    if(is_stack_empty(state->loop_structs))
    {
        zs_error("endloop not in a loop statement structure");
        return ZS_ENDLOOP_STATE_ERROR;
    }

    zs_check_execute_iteration_code(state);

    /* if the loop shouldn't be re-iterated, stop it*/
    if(!should_loop_code_be_executed(state))
    {
        /* stop loop */
        zs_end_current_loop(state);
        return ZS_ENDLOOP_STATE_LOOP_ENDED;
    }
    /* interpret loop expression again */
    data = zs_interpret_expression_copy(state, (*stack_top(state->loop_structs, zs_loop_structure)).loop_expression, &node_copy);
    
    /* test for errors */
    if(data.type == ZS_TYPE_ERROR)
    {
        zs_error("could not interpret expression for endloop");
        destroy_expr_node_tree(node_copy, state);
        return ZS_ENDLOOP_STATE_ERROR;
    }
    /* if loop expression at this stade if verified, re-iterate loop */
    if(*(int*)data.data)
    {
        /* set the cursor to the start of loop */
        zs_reiterate_loop(state);
        destroy_expr_node_tree(node_copy, state);
        return ZS_ENDLOOP_STATE_LOOP_AGAIN;
    }
    /* if loop expression at this stade is not verified, we've reached end of loop, stop the loop */
    else
    {
        destroy_expr_node_tree(node_copy, state);
        zs_end_current_loop(state);
        return ZS_ENDLOOP_STATE_LOOP_ENDED;
    }
}

int zs_crack_loop(zs_state *state)
{
    if(state->loop_structs.top < 0)
    {
        zs_error("crack not in a loop statement structure");
        return 0;
    }
    (*stack_top(state->loop_structs, zs_loop_structure)).cracking_loop = ZS_TRUE;
    return 1;
}

int zs_reiterate_loop(zs_state *state)
{
    if(state->loop_structs.top < 0)
    {
        zs_error("reit not in a loop statement structure");
        return 0;
    }
    fseek(state->fp, (*stack_top(state->loop_structs, zs_loop_structure)).loop_content_start_off, SEEK_SET);
    
    return 1;
}