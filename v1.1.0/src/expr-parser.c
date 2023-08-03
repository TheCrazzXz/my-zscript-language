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
        expr_node->data.isref = ZS_FALSE;
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
    zs_bool one_elem;
    zs_bool is_last_addition;
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

bt_node* zs_interpret_expression_input_core(char* input_expr, size_t lineno, size_t* counter, bt_node* tree, zs_bool);

size_t create_parentesis(bt_node** currentNode, bt_node** tree, zs_expr_parentesis* parentesis, zs_expr_node* temp, zs_expr_type type, zs_bool* error, char* input_where)
{
    bt_node* tempnode;
    zs_expr_parentesis expr_par;
    bt_node* where;
    size_t i;
    size_t deltai;
    zs_expr_type vec_type;

    *error = ZS_FALSE;
    
    if((*currentNode) != NULL)
    {
        if(temp)
        {
            tempnode = join_nodes(create_expr_node(temp), create_bintree_node(get_special_expr_data(ZS_EXPR_TYPE_NONE)), get_special_expr_data(type));
        }
        else
        {
            tempnode = join_nodes(create_bintree_node(get_special_expr_data(ZS_EXPR_TYPE_NONE)), create_bintree_node(get_special_expr_data(ZS_EXPR_TYPE_NONE)), get_special_expr_data(type));
        }
        

        if(parentesis->op_pending == ZS_EXPR_TYPE_NONE)
        {
            //(*currentNode)->right = create_bintree_node(make_node_data_copy(NULL, 0));
            //direct_join_nodes(tempnode, NULL, (*currentNode)->right);
            if(temp)
            {
                update_parent(*currentNode, tempnode);
                *currentNode = tempnode;
                where = tempnode->right;
            }
        
            else
            {
                update_parent(*currentNode, tempnode);
                *currentNode = tempnode;
                where = tempnode->right;
            }
            
            //(*currentNode) = (((*currentNode))->right);
        }
        else
        {
            direct_join_nodes(copy_expr_node((*currentNode)), tempnode, (*currentNode));
            (*currentNode)->node_data = get_special_expr_data(parentesis->op_pending);
            where = (((*currentNode))->right)->right;
        }
        
        
    }
    else
    {
        //expr_par.one_elem = ZS_TRUE;
        if(temp)
        {
            (*currentNode) = join_nodes(
                create_expr_node(temp),
                create_bintree_node(get_special_expr_data(ZS_EXPR_TYPE_NONE)),
                get_special_expr_data(type)
            );
        }
        else
        {
            (*currentNode) = join_nodes(
                create_bintree_node(get_special_expr_data(ZS_EXPR_TYPE_NONE)),
                create_bintree_node(get_special_expr_data(ZS_EXPR_TYPE_NONE)),
                get_special_expr_data(type)
            );
        }

        
        
        (*tree) = (*currentNode);
        where = (((*currentNode))->right);
        
    }
    if(type == ZS_EXPR_TYPE_VECTOR_INDEX || type == ZS_EXPR_TYPE_PARENTESIS)
    {
        tempnode = zs_interpret_expression_input_core(input_where, 0, &i, where, ZS_FALSE);
        i++;
        if(tempnode == NULL)
        {
            *error = ZS_TRUE;
        }
        // (*tree) = (*currentNode);
    }
    else
    {
        direct_join_nodes(create_bintree_node(get_special_expr_data(ZS_EXPR_TYPE_NONE)), NULL, where);
        where->node_data = get_special_expr_data(ZS_EXPR_TYPE_FUNCTION_ENCLOSER_LIST);
        where = where->left;
        i = 0;
        while(1)
        {
            tempnode = zs_interpret_expression_input_core(&input_where[i], 0, &deltai, where, ZS_FALSE);
            i += deltai;
            if(tempnode == NULL)
            {
                *error = ZS_TRUE;
                return i;
            }
            //printf("now at : %c\n", input_where[i]);
            if(input_where[i] == ',')
            {
                i++;
                
                where->parent->right = join_nodes(
                    create_bintree_node(get_special_expr_data(ZS_EXPR_TYPE_NONE)),
                    NULL,
                    get_special_expr_data(ZS_EXPR_TYPE_VECTOR_INITIALIZER_NEXT_ELEMENT)
                );
                where = where->parent->right->left;

            }
            else if(input_where[i] == ')' || input_where[i] == ']')
            {
                i++;
                break;
            }
            else
            {
                *error = ZS_TRUE;
                zs_error("Unknown character in function call list : %c\n", input_where[i]);
                return i;
            }
        }
    }

    

    return i;
}
size_t create_encloser_list(bt_node** currentNode, bt_node** tree, zs_expr_parentesis* parentesis, zs_expr_type type, zs_bool* error, char* input_where)
{
    size_t i;
    size_t deltai;

    bt_node* tempnode;
    bt_node* where;
    if((*currentNode) != NULL)
    {
        tempnode = join_nodes(create_bintree_node(get_special_expr_data(ZS_EXPR_TYPE_NONE)), NULL, get_special_expr_data(type));
        if(parentesis->op_pending == ZS_EXPR_TYPE_NONE)
        {
            //(*currentNode)->right = create_bintree_node(make_node_data_copy(NULL, 0));
            //direct_join_nodes(tempnode, NULL, (*currentNode)->right);
            update_parent(*currentNode, tempnode);
            *currentNode = tempnode;
            where = tempnode->left;
            //(*currentNode) = (((*currentNode))->right);
        }
        else
        {
            direct_join_nodes(copy_expr_node((*currentNode)), tempnode, (*currentNode));
            (*currentNode)->node_data = get_special_expr_data(parentesis->op_pending);
            where = (((*currentNode))->right)->left;
        }
        
    }
    else
    {
        (*currentNode) = join_nodes(
                create_bintree_node(get_special_expr_data(ZS_EXPR_TYPE_NONE)),
                NULL,
                get_special_expr_data(type)
            );
        (*tree) = (*currentNode);
        where = (((*currentNode))->left);
    }
    i = 0;
    while(1)
    {
        tempnode = zs_interpret_expression_input_core(&input_where[i], 0, &deltai, where, ZS_FALSE);
        i += deltai;
        if(tempnode == NULL)
        {
            *error = ZS_TRUE;
            return i;
        }
        //printf("now at : %c\n", input_where[i]);
        if(input_where[i] == ',')
        {
            i++;
            
            where->parent->right = join_nodes(
                create_bintree_node(get_special_expr_data(ZS_EXPR_TYPE_NONE)),
                NULL,
                get_special_expr_data(ZS_EXPR_TYPE_VECTOR_INITIALIZER_NEXT_ELEMENT)
            );
            where = where->parent->right->left;

        }
        else if(input_where[i] == '}')
        {
            i++;
            break;
        }
        else
        {
            *error = ZS_TRUE;
            zs_error("Unknown character in encloser list : %c\n", input_where[i]);
            return i;
        }
    }
    

    return i;
}

bt_node* zs_interpret_expression_input(char* input_expr, size_t lineno)
{
    return zs_interpret_expression_input_core(input_expr, lineno, NULL, NULL, ZS_FALSE);
}

bt_node* zs_interpret_expression_input_core(char* input_expr, size_t lineno, size_t* counter, bt_node* tree, zs_bool in_encloser_list)
{
    size_t i;
    size_t len;
    size_t currsize;
    zs_expr_node d1;
    zs_expr_node d2;
    zs_expr_node temp;
    zs_expr_node currentExprNode;
    zs_bool firstcore;

    bt_node* tempnode;
    bt_node* currentNode;
    zs_expr_type operation;
    zs_bool error;
    zs_expr_parentesis expr_par;
    zs_expr_parentesis parentesis;

    firstcore = tree == NULL;

    currentNode = tree;

    d1.expr_type = ZS_EXPR_TYPE_NONE;
    d2.expr_type = ZS_EXPR_TYPE_NONE;
    temp.expr_type = ZS_EXPR_TYPE_NONE;

    len = strlen(input_expr);
    

    parentesis.op_pending = ZS_EXPR_TYPE_NONE;
    parentesis.one_elem = ZS_TRUE;
    parentesis.is_last_addition = ZS_FALSE;

    i = 0;

    if(len == 0 || input_expr[i] == ')')
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

            if(input_expr[i] == '(')
            {
                i++;
                i += create_parentesis(&currentNode, &tree, &parentesis, &temp, ZS_EXPR_TYPE_FUNCTION_CALL, &error, &input_expr[i]);
                if(error)
                {
                    if(tree)
                    {
                        free_bintree_node(tree);
                    }
                    return NULL;
                }
                continue;
            }
            else if(input_expr[i] == '[')
            {
                i++;
                i += create_parentesis(&currentNode, &tree, &parentesis, &temp, ZS_EXPR_TYPE_VECTOR_INDEX, &error, &input_expr[i]);
                if(error)
                {
                    if(tree)
                    {
                        free_bintree_node(tree);
                    }
                    return NULL;
                }
                continue;
            }

            else if(parentesis.op_pending == ZS_EXPR_TYPE_ADDITION || parentesis.op_pending == ZS_EXPR_TYPE_SUBTRACTION)
            {
                direct_join_nodes(copy_expr_node(currentNode), create_expr_node(&temp), currentNode);
                currentNode->node_data = get_special_expr_data(parentesis.op_pending);

                parentesis.is_last_addition = ZS_TRUE;

                /*if(input_expr[i] == '*' || input_expr[i] == '/')
                {
                    currentNode = (currentNode)->right;
                    printf("prev multiplication : currentNode->parent : %p\n", currentNode->parent);
                }*/
                
            }
            
            else if(parentesis.op_pending == ZS_EXPR_TYPE_NONE)
            {
                
                if(parentesis.one_elem == ZS_TRUE)
                {
                    tempnode = create_expr_node(&temp);
                    
                    if(currentNode)
                    {
                        if(update_parent(currentNode, tempnode) == 0)
                        {
                            tree = tempnode;
                        }
                    }
                    else
                    {
                        tree = tempnode;
                    }
                    
                    currentNode = tempnode;
                }

                parentesis.is_last_addition = ZS_FALSE;
                
                if(parentesis.one_elem == ZS_TRUE && i >= len - 1 && firstcore)
                {   
                    break;
                }
            }
            else if(parentesis.op_pending == ZS_EXPR_TYPE_MULTIPLICATION
            || parentesis.op_pending == ZS_EXPR_TYPE_DIVISION
            || parentesis.op_pending == ZS_EXPR_TYPE_POWER)
            {
                if(parentesis.is_last_addition == ZS_TRUE)
                {
                    currentNode = currentNode->right;
                }
                direct_join_nodes(copy_expr_node(currentNode), create_expr_node(&temp), currentNode);
                currentNode->node_data = get_special_expr_data(parentesis.op_pending);
                parentesis.is_last_addition = ZS_FALSE;
            }
            else
            {   
                direct_join_nodes(copy_expr_node(currentNode), create_expr_node(&temp), currentNode);
                currentNode->node_data = get_special_expr_data(parentesis.op_pending);
                parentesis.is_last_addition = ZS_FALSE;
            }
            parentesis.op_pending = ZS_EXPR_TYPE_NONE;
        }
        operation = zs_str_to_expr_type(&input_expr[i], &currsize);
        if(operation != ZS_EXPR_TYPE_NONE)
        {
            parentesis.one_elem = ZS_FALSE;
       
            if(operation == ZS_EXPR_TYPE_ADDITION || operation == ZS_EXPR_TYPE_SUBTRACTION)
            {
                parentesis.is_last_addition = ZS_TRUE;
            }
            parentesis.op_pending = operation;
            i += currsize;
        }
        /*else if(input_expr[i] == '(')
        {
            printf("pushed node : %p\n", currentNode);
            ptrstack_push(&savedNodes, currentNode);
            currentNode = join_nodes(currentNode, get_expr_node_int(0), get_special_expr_data(ZS_EXPR_TYPE_ADDITION));
            currentNode = &(currentNode)->right;
            
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
        
        
        /*else if(input_expr[i] == ']')
        {
            currentNode = (currentNode)->parent;
            i++;
        }*/
        
        else if(input_expr[i] == '(')
        {
            i++;
            i += create_parentesis(&currentNode, &tree, &parentesis, NULL, ZS_EXPR_TYPE_PARENTESIS, &error, &input_expr[i]);
            if(error)
            {
                if(tree)
                {
                    free_bintree_node(tree);
                }
                return NULL;
            }
        }
        else if(input_expr[i] == '{')
        {
            i++;
            i += create_encloser_list(&currentNode, &tree, &parentesis, ZS_EXPR_TYPE_VECTOR_INITIALIZER, &error, &input_expr[i]);
            if(error)
            {
                if(tree)
                {
                    free_bintree_node(tree);
                }
                return NULL;
            }
        }
        else if(input_expr[i] == ',' || input_expr[i] == '}' || input_expr[i] == ')' || input_expr[i] == ']')
        {
            if(firstcore)
            {
                zs_error("trying to put a comma or closing encloser list that doesn't exist in the first place");
                return NULL;
            }
            if(counter != NULL)
            {
                *counter = i;
            }
            
            return tree;
        }
        /*else if(input_expr[i] == ']')
        {
            if(firstcore)
            {
                zs_error("trying to close a parentesis that wasn't opened in the first place");
                return NULL;
            }
            i++;
            if(counter != NULL)
            {
                *counter = i;
            }
            
            return tree;
        }*/
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

