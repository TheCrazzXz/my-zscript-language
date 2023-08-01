#include "expr-parser.h"
#include "args-parser.h"
#include "expr-tree.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "memory/bintree.h"
#include "memory/stack.h"
#include "zs-data.h"
#include "zs-error.h"
#include "zs-expression.h"
#include "zs-functions.h"
#include "io-utils.h"
#include "memory/ptrstack.h"
#include "zs-vector.h"
#include <time.h>

/* reads a int from an input, returns the number of digits */
size_t read_number(char* input, void** output, size_t* size, zs_var_type* type)
{
    size_t i;
    size_t len;
    char* digit_str;

    *size = sizeof(int);
    *type = ZS_TYPE_INT;

    for(i = 0, len = 0 ; ;)
    {
        /* current character is a digit */
        if(is_ascii_digit(input[i]) || (is_ascii_int_digit(input[i]) && i == 0))
        {
            i++;
            len++;
        }
        /* floating point */
        else if(input[i] == '.')
        {
            *size = sizeof(float);
            *type = ZS_TYPE_FLOAT;
            i++;
            len++;
        }
        /* end of digits */
        else
        {
            digit_str = malloc(len + 1);
            strncpy(digit_str, input, len);
            digit_str[len] = '\0';

            *output = malloc(*size);

            if(*type == ZS_TYPE_INT)
            {
                **(int**)output = atoi(digit_str);
            }
            else
            {
                **(float**)output = atof(digit_str);
            }
           
            free(digit_str);
            return len;
        }
    }
}
size_t read_varname(char* input, char** output)
{
    size_t i;
    size_t len;

    for(i = 0, len = 0 ; ;)
    {
        /* current character is a character */
        if(is_variable_character(input[i]))
        {
            i++;
            len++;
        }
        /* end of characters */
        else
        {
            *output = malloc(len + 1);
            strncpy(*output, input, len);
            (*output)[len] = '\0';
            return len;
        }
    }
}
ssize_t read_string_literal(char* input, char** output)
{
    size_t i;
    size_t len;
    int escaping;

    escaping = 0;

    if(*input != '"')
    {
        return 0;
    }

    for(i = 1, len = 0 ; ;)
    {
        if(input[i] == '\\')
        {
            escaping = 1;
        }
        else
        {
            ALLOCATION_FOR_NEW_ELEM_VECTOR(*output, len, sizeof(char));
            (*output)[len - 1] = input[i]; 
        }
        /* end of string literal */
        if(input[i] == '"')
        {
            if(!escaping)
            {
                //*output = malloc(len + 1);
                //strncpy(*output, &input[1], len);
                (*output)[len - 1] = '\0';
                return (ssize_t)(len + 1);
            }
            else
            {
                escaping = 0;
                i++;
            }
            
        }
        else if(input[i] == '\0')
        {
            zs_error("unmatched \"");
            return -1;
        }
        else
        {
            i++;
        }
    }
}

#define randint(min, max) (rand() % (max + 1 - min)) + min

size_t zs_read_next_data(char* input_expr, size_t i, zs_expr_node* expr_node, zs_bool* error)
{
    size_t j;
    size_t currsize;
    *error = ZS_FALSE;
    if(is_ascii_int_digit(input_expr[i]))
    {
        currsize = read_number(&input_expr[i], &expr_node->data.data, &expr_node->data.size, &expr_node->data.type);
        expr_node->expr_type = ZS_EXPR_TYPE_FOLLOW_VAR_TYPE;
        expr_node->data.isref = ZS_FALSE;
    }
    else if(strcmp(&input_expr[i], "vector") == 0)
    {
        currsize = strlen("vector");
        zs_create_alloc_expr_node_data(expr_node, ZS_TYPE_VECTOR);
    }
    else if(strncmp(&input_expr[i], "readint", strlen("readint")) == 0)
    {
        currsize = strlen("readint");
        zs_create_alloc_expr_node_data(expr_node, ZS_TYPE_INT);

        zs_readint(expr_node->data.data);
    }
    else if(strncmp(&input_expr[i], "randint", strlen("randint")) == 0)
    {
        currsize = strlen("randint");
        zs_create_alloc_expr_node_data(expr_node, ZS_TYPE_INT);

        *(int*)expr_node->data.data = randint(1, 100);
    }
    else if(strncmp(&input_expr[i], "readfloat", strlen("readfloat")) == 0)
    {
        currsize = strlen("readfloat");
        
        zs_create_alloc_expr_node_data(expr_node, ZS_TYPE_FLOAT);

        zs_readfloat(expr_node->data.data);
    }
    else if(strncmp(&input_expr[i], "readstr", strlen("readstr")) == 0)
    {
        currsize = strlen("readstr");

        zs_create_alloc_expr_node_data(expr_node, ZS_TYPE_STRING);
        expr_node->data.size = zs_readstring((char**)&expr_node->data.data) + 1;
    }
    else if(is_variable_character(input_expr[i]))
    {
        currsize = read_varname(&input_expr[i], (char**)&expr_node->data.data);
        /*if(strncmp(&input_expr[i + currsize], "()", 2) == 0)
        {
            expr_node->data.size = currsize + 1;
            expr_node->expr_type = ZS_EXPR_TYPE_FUNCTION_CALL;
            expr_node->data.isref = ZS_TRUE;
            currsize += 2;
        }
        else
        {*/
            expr_node->data.size = currsize + 1;
            expr_node->expr_type = ZS_EXPR_TYPE_VARIABLE;
            expr_node->data.isref = ZS_TRUE;
        //}
        
    }
    else if(input_expr[i] == '"')
    {
        zs_create_alloc_expr_node_data(expr_node, ZS_TYPE_STRING);
        currsize = read_string_literal(&input_expr[i], (char**)&expr_node->data.data);
        if(currsize == -1)
        {
            zs_error("couldn't read string litteral");
            *error = ZS_TRUE;
            return 0;
        }
        
        expr_node->data.size = currsize - 1;
    }
    else
    {
        return 0;
    }
    return currsize;
}

#define create_expr_node(data) create_bintree_node(make_node_data_copy((data), sizeof(zs_expr_node)))

typedef struct __zs_expr_parentesis_t
{
    zs_expr_type op_pending;
    bt_node* prev_node;
} zs_expr_parentesis;

stack_data get_expr_parentesis_struct_stack_data(zs_expr_parentesis expr_par)
{
    stack_data data;

    data.size = sizeof(zs_expr_parentesis);
    data.data = malloc(data.size);
    memcpy(data.data, &expr_par, data.size);

    return data;
}

#define expr_parentesis_stack_top(expr_par_stack) (stack_top(expr_par_stack, zs_expr_parentesis))


bt_node* zs_interpret_expression_input(char* input_expr, size_t lineno)
{
    size_t i;
    size_t len;
    size_t currsize;
    zs_expr_node d1;
    zs_expr_node d2;
    zs_expr_node temp;
    zs_expr_node currentExprNode;

    bt_node* tree;
    bt_node* tempnode;
    bt_node* currentNode;
    stack parentesis;
    zs_expr_type operation;
    zs_bool one_elem;
    zs_bool error;
    zs_expr_parentesis expr_par;

    currentNode = tree;
    tree = NULL;

    d1.expr_type = ZS_EXPR_TYPE_NONE;
    d2.expr_type = ZS_EXPR_TYPE_NONE;
    temp.expr_type = ZS_EXPR_TYPE_NONE;

    len = strlen(input_expr);
    one_elem = ZS_TRUE;

    parentesis = create_stack();
    expr_par.op_pending = ZS_EXPR_TYPE_NONE;
    expr_par.prev_node = NULL;
    stack_push(&parentesis, get_expr_parentesis_struct_stack_data(expr_par));

    if(len == 0)
    {
        zs_error("expression can't be nothing");
        return NULL;
    }

    for(i = 0 ; i < len ;)
    {
        currsize = zs_read_next_data(input_expr, i, &temp, &error);
        if(error)
        {
            zs_error("error while reading next data at line %lu", lineno);
            if(tree)
            {
                free_bintree_node(tree);
            }
            return NULL;
        }


        if(currsize >= 1)
        {
            i += currsize;
            if(expr_parentesis_stack_top(parentesis)->op_pending == ZS_EXPR_TYPE_ADDITION || expr_parentesis_stack_top(parentesis)->op_pending == ZS_EXPR_TYPE_SUBTRACTION)
            {
                *currentNode = join_nodes(*currentNode, create_expr_node(&temp), get_special_expr_data(expr_parentesis_stack_top(parentesis)->op_pending));                
                
                if(input_expr[i] == '*' || input_expr[i] == '/')
                {
                    currentNode = &(*currentNode)->right;
                }
                
            }
            else if(input_expr[i] == '[')
            {
                *currentNode = join_nodes(create_expr_node(&temp), create_bintree_node(make_node_data_copy(NULL, 0)), get_special_expr_data(ZS_EXPR_TYPE_VECTOR_INDEX));
                currentNode = &(*currentNode)->right;
                i++;
                continue;
            }

            else if(expr_parentesis_stack_top(parentesis)->op_pending == ZS_EXPR_TYPE_NONE)
            {
                printf("no op pending\n");
                if(one_elem == ZS_TRUE && i >= len - 1)
                {   
                    *currentNode = create_expr_node(&temp);
                    
                    break;
                }
                else if(one_elem == ZS_TRUE)
                {
                    *currentNode = create_expr_node(&temp);
                }
                else
                {
                    *currentNode = create_expr_node(&temp);
                }
                
            }
            else
            {
                *currentNode = join_nodes(*currentNode, create_expr_node(&temp), get_special_expr_data(expr_parentesis_stack_top(parentesis)->op_pending));                
            }
            expr_parentesis_stack_top(parentesis)->op_pending = ZS_EXPR_TYPE_NONE;
        }
        operation = zs_str_to_expr_type(&input_expr[i], &currsize);
        if(operation != ZS_EXPR_TYPE_NONE)
        {
            one_elem = ZS_FALSE;
       
            expr_parentesis_stack_top(parentesis)->op_pending = operation;
            i += currsize;
        }
        /*else if(input_expr[i] == '(')
        {
            printf("pushed node : %p\n", currentNode);
            ptrstack_push(&savedNodes, currentNode);
            *currentNode = join_nodes(*currentNode, get_expr_node_int(0), get_special_expr_data(ZS_EXPR_TYPE_ADDITION));
            currentNode = &(*currentNode)->right;
            
            //savedNode = currentNode;
            i++;
        }
        else if(input_expr[i] == ')')
        {

            //print_bintree_node(*savedNode, print_expr_node, 0);
            currentNode = ptrstack_pop(&savedNodes);
            printf("restored node : %p\n", currentNode);
            i++;
        }*/
        else if(input_expr[i] == ')')
        {
            printf("outsize parameter\n");
            currentNode = ;
            i++;
        }
        else if(input_expr[i] == '(')
        {
            

            *currentNode = join_nodes(copy_expr_node(*currentNode), join_nodes(create_expr_node(&temp), create_bintree_node(make_node_data_copy(NULL, 0)), get_special_expr_data(ZS_EXPR_TYPE_FUNCTION_CALL)), get_special_expr_data(expr_parentesis_stack_top(parentesis)->op_pending));
            
            expr_par.op_pending = expr_parentesis_stack_top(parentesis)->op_pending;
            expr_par.prev_node = (*currentNode)->left;

            stack_push(&parentesis, get_expr_parentesis_struct_stack_data(expr_par));

            currentNode = &((*currentNode)->right)->right;
            print_bt_node((*currentNode), print_expr_node, 0);
            i++;
            continue;
        }
        else if(input_expr[i] == ']')
        {
            currentNode = &(*currentNode)->parent;
            i++;
        }
        else if(input_expr[i] == '\0')
        {
            break;
        }
        else
        {
            zs_error("unknown character for expression at line %lu : '%c' (ascii : 0x%x)", lineno, input_expr[i], (unsigned char)input_expr[i]);
            if(tree)
            {
                free_bintree_node(tree);
            }
            return NULL;
        }
    }

    return tree;
}

