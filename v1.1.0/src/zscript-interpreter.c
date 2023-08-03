#include "zscript-interpreter.h"

#include "expr-tree.h"
#include "io-utils.h"
#include "args-parser.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "memory/memutils.h"
#include "zs-expression.h"
#include "zs-function.h"
#include "zs-loop.h"
#include "zs-conditions.h"
#include "memory/bintree.h"

#include "zs-data.h"
#include "zs-error.h"
#include "zs-variable.h"
#include "expr-parser.h"
#include "zs-functions.h"
#include "memory/stack.h"
#include "zs-vector.h"
#include <time.h>
#include "strutils.h"
#include "config.h"

void zs_stop(zs_state* state)
{
    size_t i;
    size_t j;
    for(i = 0 ; i < state->nVars ; i++)
    {
        clean_data_if_not_reference(&state->variables[i].data, state);
    }
    for(i = 0 ; i < state->nFuncs ; i++)
    {
        free(state->functions[i].name);
        for(j = 0 ; j < state->functions[i].nParemters ; j++)
        {
            free(state->functions[i].parameters[j]);
        }
        free(state->functions[i].parameters);
        state->ndel++;
    }
    free(state->variables);
    free(state->functions);
    if(DO_DEBUG(1))
    {
        printf("DEBUG : number of deleted data : %lu : %lu bytes \n", state->ndel, state->totaldelsize);
    }
}



zs_bool should_code_be_executed(zs_state* state)
{
    if(!is_stack_empty(state->cond_structs))
    {
        if(!should_condition_code_be_executed(state))
        {
            return ZS_FALSE;
        }
    }
    if(!is_stack_empty(state->loop_structs))
    {
        if(!should_loop_code_be_executed(state))
        {
            return ZS_FALSE;
        }
    }
    return ZS_TRUE;
    
}


/* keywords that execute whenever normal code shouldn't be executed because of loop structure or conditionnal structure */
const char* keywords_unconditionnaly_execute[] = { "if", "loop", "else", "endloop", "endif", "for", NULL};

zs_bool is_keyword_that_unconditionnaly_execute(char* word)
{
    size_t i;
    for(i = 0 ; i < (sizeof(keywords_unconditionnaly_execute) / sizeof(char*)) ; i++)
    {
        if(keywords_unconditionnaly_execute[i] == NULL)
        {
            break;
        }
        if(strcmp(keywords_unconditionnaly_execute[i], word) == 0)
        {
            return ZS_TRUE;
        }
    }
    return ZS_FALSE;
}
interprl_rcode zs_interpret_line(char* line, zs_state* state, size_t lineno)
{
    size_t nWords;
    char** words;
    zs_var_type var_type;
    int idx;
    ssize_t index;
    ssize_t index2;
    size_t count;
    zs_data data;
    zs_data data2;
    char* str;
    char* varname;
    int newline;
    zs_conditionnal_structure cs;
    zs_loop_structure ls;
    bt_node* node_copy;
    size_t i;
    zs_endloop_state endloop_state;
    zs_endfuncdef_state endfuncdef_state;
    zs_expr_type expr_type;
    char* init_code;
    char* it_code;
    zs_bool comment;
    zs_bool running;
    interprl_rcode retcode;
    FILE* thisfp;

    words = parse_command_args(line, &nWords);

    // printf("%s\n", line);
    // print_command_args(words, nWords);

    if(strcmp(words[0], "endef") == 0)
    {
        endfuncdef_state = zs_end_function_define(state);
        if(endfuncdef_state == ZS_ENDFUNCDEF_STATE_END)
        {
            return INTERPRET_LINE_RETURN_ENDEF;
        }
        else if(endfuncdef_state == ZS_ENDFUNCDEF_STATE_DEFINED)
        {
            return INTERPRET_LINE_RETURN_ENDEF;
        }
        else
        {
            return INTERPRET_LINE_RETURN_ERROR;
        }
    }
    else
    {
        if(state->defining_function)
        {
            return INTERPRET_LINE_RETURN_SUCCESS;
        }
        
    }
    if(!is_keyword_that_unconditionnaly_execute(words[0]))
    {
        /* check if code should be executed */   
        if(!zs_check_condition_code_to_be_executed(state))
        {
            return INTERPRET_LINE_RETURN_SUCCESS;
        }
        if(!zs_check_loop_code_to_be_executed(state))
        {
            return INTERPRET_LINE_RETURN_SUCCESS;
        }
    }
    
    if(nWords == 0)
    {
        return INTERPRET_LINE_RETURN_NOTHING;
    }
    /* variable printing */
    if(strncmp(words[0], "print", strlen("print")) == 0)
    {
        if(nWords < 2)
        {
            zs_error("print takes at least one argument");
            return INTERPRET_LINE_RETURN_ERROR;
        }

        if(zs_print(state, &words[1], nWords - 1, ((words[0][5] == 'r') ? ZS_FALSE : ZS_TRUE), lineno) == 0)
        {
            zs_error("Failed at print");
            return INTERPRET_LINE_RETURN_ERROR;
        }

    }
    else if(strcmp(words[0], "defunc") == 0)
    {
        if(nWords < 2)
        {
            zs_error("defunc needs the name of the declared function");
            return INTERPRET_LINE_RETURN_ERROR;
        }
        zs_define_function_here(state, words[1], &words[2], nWords - 2);
    }
    /* conditionnal statement */
    else if(strcmp(words[0], "if") == 0)
    {
        /* check if the if statement should be shadowed */
        if((zs_check_shadow_if(state)))
        {
            return INTERPRET_LINE_RETURN_NOTHING;
        }

        if(zs_new_conditionnal_structure(state, zs_interpret_expression_input(words[1], lineno)) == 0)
        {
            return INTERPRET_LINE_RETURN_ERROR;
        }
    }
    /* end conditionnal structure */
    else if(strcmp(words[0], "endif") == 0)
    {
        if(zs_check_end_conditionnal_statement(state))
        {
            return INTERPRET_LINE_RETURN_NOTHING;
        }
        else
        {
            return INTERPRET_LINE_RETURN_ERROR;
        }
    }
    /* end loop structure */
    else if(strcmp(words[0], "endloop") == 0)
    {
        endloop_state = zs_check_endloop(state);

        if(endloop_state == ZS_ENDLOOP_STATE_ERROR)
        {
            return INTERPRET_LINE_RETURN_ERROR;
        }
        else if(endloop_state == ZS_ENDLOOP_STATE_LOOP_AGAIN)
        {
            return INTERPRET_LINE_RETURN_NOTHING;
        }
        else if(endloop_state == ZS_ENDLOOP_STATE_LOOP_ENDED)
        {
            return INTERPRET_LINE_RETURN_SUCCESS;
        }
    }
    /* else keyword */
    else if(strcmp(words[0], "else") == 0)
    {
        if(zs_new_else_statement(state))
        {
            return INTERPRET_LINE_RETURN_NOTHING;
        }
        else
        {
            return INTERPRET_LINE_RETURN_ERROR;
        }
        
    }
    else if(strcmp(words[0], "loop") == 0)
    {
        if(zs_new_loop(state, zs_interpret_expression_input(words[1], lineno), NULL) == 0)
        {
            return INTERPRET_LINE_RETURN_ERROR;
        }
    }
    /* for i = 0 ; i<2 ; i += 1*/
    else if(strcmp(words[0], "for") == 0)
    {
        index = chridx(line, ';');
        if(index == -1)
        {
            zs_error("Missing ; after initialization code in for loop");
            return INTERPRET_LINE_RETURN_ERROR;
        }
        index2 = chridx((char*)((unsigned long)line + (unsigned long)index + 1), ';');
        if(index2 == -1)
        {
            zs_error("Missing ; after initialization code in for loop");
            return INTERPRET_LINE_RETURN_ERROR;
        }
        index2 += index;
        init_code = strdupc((char*)((unsigned long)line + strlen("for ")), ';', 1);
        init_code[strlen(init_code) - 1] = '\0';
        it_code = strdupc((char*)((unsigned long)line + index2 + 3), '\x00', 1);

        if(zs_interpret_line(init_code, state, lineno) == INTERPRET_LINE_RETURN_ERROR)
        {
            zs_error("failed for loop");
            return INTERPRET_LINE_RETURN_ERROR;
        }

        free(init_code);

        if(zs_new_loop(state, zs_interpret_expression_input(words[(size_t)strarrstr(words, nWords, 0, ";") + 1], lineno), it_code) == 0)
        {
            return INTERPRET_LINE_RETURN_ERROR;
        }
    }
    else if(strcmp(words[0], "crack") == 0)
    {
        if(zs_crack_loop(state) == 0)
        {
            return INTERPRET_LINE_RETURN_ERROR;
        }
    }
    else if(strcmp(words[0], "reit") == 0)
    {
        zs_check_execute_iteration_code(state);
        if(zs_reiterate_loop(state) == 0)
        {
            return INTERPRET_LINE_RETURN_ERROR;
        }
    }
    else if(strcmp(words[0], "return") == 0)
    {
        if(is_stack_empty(state->call_stack))
        {
            zs_error("trying to return in something not a function");
            return INTERPRET_LINE_RETURN_ERROR;
        }

        if(nWords >= 2)
        {
            data = zs_interpret_expression(state, zs_interpret_expression_input(words[1], lineno), ZS_TRUE);
        
            if(data.type == ZS_TYPE_ERROR)
            {
                zs_error("couldn't interpret expression");
                return INTERPRET_LINE_RETURN_ERROR;
            }

            state->function_return = data;
        }
        else
        {
            zs_create_alloc_data(&state->function_return, ZS_TYPE_NONE);
        }
        

        zs_end_function_define(state);

        return INTERPRET_LINE_RETURN_RETURN;
    }
    /*else if(strcmp(words[0], "include") == 0)
    {
        if(nWords < 2)
        {
            zs_error("Error : need to specify what zscript file to include to the program");
            return INTERPRET_LINE_RETURN_ERROR;
        }

        comment = 0;
        running = 1;

        thisfp = state->fp;

        state->fp = fopen(words[1], "r");
        if(!state->fp)
        {
            state->fp = thisfp;
            return INTERPRET_LINE_RETURN_ERROR;
        }*/

        /* execute function code until return */
        /*while(!feof(state->fp))
        {
            retcode = zs_read_line(state, &comment, &running, 0);
            if(retcode == INTERPRET_LINE_RETURN_ERROR)
            {
                printf("error in module");
                state->fp = thisfp;
                return INTERPRET_LINE_RETURN_ERROR;
            }
            else if(retcode == INTERPRET_LINE_RETURN_EXIT)
            {
                state->fp = thisfp;
                return INTERPRET_LINE_RETURN_SUCCESS;
            }
        }

        state->fp = thisfp;
    }*/
    /* exit */
    else if(strcmp(words[0], "exit") == 0)
    {
        printf("exit\n");
        return INTERPRET_LINE_RETURN_EXIT;
    }
    else if(nWords >= 3 && strcmp(words[1], "=") == 0)
    {

        /* variable modifying */
        if(variable_exists(state, words[0]))
        {
            index = zs_get_variable_index(state, words[0]);
            
            if(zs_alter_variable(state, index, words[2], lineno) == 0)
            {
                zs_error("couldn't create variable");
                return INTERPRET_LINE_RETURN_ERROR;
            }
        }
        else
        {
            zs_error("variable %s doesn't exist", words[0]);
            return INTERPRET_LINE_RETURN_ERROR;
        }

        
    }
    else if(nWords >= 3 && (strcmp(words[1], "+=") == 0 || strcmp(words[1], "-=") == 0))
    {

        /* variable modifying */
        if(variable_exists(state, words[0]))
        {
            index = zs_get_variable_index(state, words[0]);

            if(strcmp(words[1], "+=") == 0)
            {
                expr_type = ZS_EXPR_TYPE_ADDITION;
            }
            else if(strcmp(words[1], "-=") == 0)
            {
                expr_type = ZS_EXPR_TYPE_SUBTRACTION;
            }
            
            if(zs_operate_variable(state, index, words[2], expr_type, lineno) == 0)
            {
                zs_error("couldn't operate variable");
                return INTERPRET_LINE_RETURN_ERROR;
            }
        }
        else
        {
            zs_error("variable %s doesn't exist", words[0]);
            return INTERPRET_LINE_RETURN_ERROR;
        }   
    }
    else if(nWords >= 4 && strcmp(words[2], "=") == 0)
    {
        /* new variable assigning */
        var_type = zs_string_to_type(words[0]);
        if(var_type == ZS_TYPE_ERROR)
        {
            zs_error("data type doesn't exist");
            return INTERPRET_LINE_RETURN_ERROR;
        }
        if(zs_create_variable(state, words[1], words[3], var_type, lineno) == 0)
        {
            zs_error("variable was not created");
            return INTERPRET_LINE_RETURN_ERROR;
        }
        return INTERPRET_LINE_RETURN_SUCCESS;
    }
    /* var[ <expression : index> ] = <expression> */
    else if(nWords >= 5 && strcmp(words[3], "=") == 0)
    {
        index = chridx(words[0], '[');
        if(index == -1)
        {
            zs_error("not affecting expression to list element");
            return INTERPRET_LINE_RETURN_ERROR;
        }
        varname = strdupc(words[0], '[', 0);
        data = zs_interpret_expression(state, zs_interpret_expression_input(words[1], lineno), ZS_TRUE);
        

        if(data.type == ZS_TYPE_ERROR)
        {
            zs_error("couldn't interpret expression");
            return INTERPRET_LINE_RETURN_ERROR;
        }
        if(data.type != ZS_TYPE_INT)
        {
            zs_error("trying to use a non integer number as index for vector");
            clean_data_if_not_reference(&data, state);
            free(varname);

            return INTERPRET_LINE_RETURN_ERROR;
        }
        
        idx = *(int*)(data.data);

        if(strcmp(words[2], "]") != 0)
        {
            zs_error("closing bracket unmatched");
            return INTERPRET_LINE_RETURN_ERROR;
        }

        if(variable_exists(state, varname))
        {
            index = zs_get_variable_index(state, varname);

            if(state->variables[index].data.type != ZS_TYPE_VECTOR && state->variables[index].data.type != ZS_TYPE_STRING)
            {
                zs_error("trying to set an element of a variable not a vector or string");
                clean_data_if_not_reference(&data, state);
                free(varname);

                return INTERPRET_LINE_RETURN_ERROR;
            }

            data2 = zs_interpret_expression(state, zs_interpret_expression_input(words[4], lineno), ZS_TRUE);
        
            if(data2.type == ZS_TYPE_ERROR)
            {
                zs_error("couldn't interpret expression");
                clean_data_if_not_reference(&data2, state);
                clean_data_if_not_reference(&data, state);
                free(varname);
                return INTERPRET_LINE_RETURN_ERROR;
            }
            if(state->variables[index].data.type == ZS_TYPE_VECTOR)
            {
                zs_set_element((zs_vector*)state->variables[index].data.data, *(int*)data.data, data2, state);
            }
            else if(state->variables[index].data.type == ZS_TYPE_STRING)
            {
                if(data2.type != ZS_TYPE_STRING)
                {
                    zs_error("trying to set a character of a string to something not a character");
                    clean_data_if_not_reference(&data2, state);
                    clean_data_if_not_reference(&data, state);
                    free(varname);
                    return INTERPRET_LINE_RETURN_ERROR;
                }

                if(idx + 1 > state->variables[index].data.size)
                {
                    zs_error("index of string is too big, can modify at most the index after the size of the string");;
                    clean_data_if_not_reference(&data2, state);
                    clean_data_if_not_reference(&data, state);
                    free(varname);
                    return INTERPRET_LINE_RETURN_ERROR;
                }

                ((char*)state->variables[index].data.data)[idx] = *(char*)data2.data;

                if(idx + 1 == state->variables[index].data.size)
                {
                    //if(state->variables[index].data.size == 0)
                    ALLOCATION_FOR_NEW_ELEM_VECTOR(state->variables[index].data.data, state->variables[index].data.size, sizeof(char));
                    ((char*)state->variables[index].data.data)[state->variables[index].data.size - 1] = '\0';
                    
                } 
            }
        }
        else
        {
            zs_error("variable %s doesn't exist", words[0]);
            clean_data_if_not_reference(&data, state);
            free(varname);

            return INTERPRET_LINE_RETURN_ERROR;
        }
        
        clean_data_if_not_reference(&data, state);

        free(varname);

    }
    else
    {
        if(state->nFuncs > 0)
        {
            if(zs_function_exists(state, words[0]))
            {
                zs_call_function(state, (size_t)zs_get_function_index(state, words[0]), NULL, 0);
                return INTERPRET_LINE_RETURN_SUCCESS;
            }
        }
        data = zs_interpret_expression(state, zs_interpret_expression_input(words[0], lineno), ZS_TRUE);
        
        if(data.type == ZS_TYPE_ERROR)
        {
            zs_error("couldn't interpret expression");
            return INTERPRET_LINE_RETURN_ERROR;
        }

        if(state->fp == stdin)
        {
            str = zs_get_data_as_string(&data, 1);

            if(str != NULL)
            {
                puts(str);
                free(str);
            }
        
        }

        
        clean_data_if_not_reference(&data, state);

        
        
    }
    
    parse_command_args_cleanup(words, nWords);

    return INTERPRET_LINE_RETURN_SUCCESS;
}

interprl_rcode zs_read_line(zs_state* state, zs_bool* comment, zs_bool* running, size_t lineno)
{
    char* currentLine;
    char* line;
    interprl_rcode rcode;
    rcode = INTERPRET_LINE_RETURN_NOTHING;

    /* read a line from fp */
    dynamic_input(&currentLine, state->fp);
    line = currentLine;

    //printf("ftell(fp) : %lu\n", ftell(state->fp));
    /* ignore tab */
    while(1)
    {
        if(*currentLine == '\t' || *currentLine == ' ')
        {
            currentLine++;
        }
        else
        {
            break;
        }
    }
    

    if(strcmp(currentLine, "//") == 0)
    {
        if(*comment == 1)
        {
            *comment = 0;
        }
        else
        {
            *comment = 1;
        }
        
    }
    /* if line is not a comment then interpret the line */
    if(currentLine[0] != '/' && !(*comment) && currentLine[0] != '\x00')
    {
        rcode = zs_interpret_line(currentLine, state, lineno);
    }
    free(line); 
    return rcode;
}

void zs_intepret_code(FILE* fp)
{
    zs_state state;
    char* currentLine;
    char* line;
    interprl_rcode rcode;
    zs_bool comment;
    zs_bool code_exec;
    code_exec = ZS_TRUE;
    size_t lineno;
    zs_bool running;

    srand(time(NULL));

    lineno = 1;
    comment = 0;
    running = ZS_TRUE;
    

    memset(&state, 0, sizeof(zs_state));
    state.fp = fp;
    state.cond_structs = create_stack();
    state.loop_structs = create_stack();
    state.call_stack = create_stack();

    while(!feof(fp) && running)
    {
        if(fp == stdin)
        {
            if(!comment && code_exec && state.cond_structs.top < 0 && state.loop_structs.top < 0 && !state.defining_function)
            {
                printf(">> ");
            }
            
        }
        
        rcode = zs_read_line(&state, &comment, &running, lineno);

        if(rcode == INTERPRET_LINE_RETURN_EXIT)
        {
            break;
        }
        else if(rcode == INTERPRET_LINE_RETURN_ERROR)
        {
            fprintf(stderr, "Error happened\n");
            if(fp != stdin)
            {
                fprintf(stderr, "ZSCRIPT : Exiting...\n");
                
                running = ZS_FALSE;
            }
            
        }

        
        lineno++;
    }
    zs_stop(&state);
    free_stack(&state.cond_structs);
    free_stack(&state.loop_structs);
    free_stack(&state.call_stack);
}