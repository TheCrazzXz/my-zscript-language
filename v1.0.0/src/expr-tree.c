#include "expr-tree.h"
#include "config.h"
#include "memory/bintree.h"
#include "memory/stack.h"
#include "zs-error.h"
#include "zs-data.h"
#include "zs-expression.h"
#include "zs-function.h"
#include "zs-variable.h"
#include <limits.h>
#include <string.h>
#include "arithmetic.h"
#include "zs-vector.h"
#include "zscript-interpreter.h"
#include "io-utils.h"
#include "zs-functions.h"
/* #define EXPRESSION_TREE_DEBUG_ENABLED */

zs_expr_node expr_node_int(int number)
{
    zs_expr_node node;

    node.expr_type = ZS_EXPR_TYPE_FOLLOW_VAR_TYPE;
    node.data.type = ZS_TYPE_INT;
    node.data.data = malloc(sizeof(int));
    memcpy(node.data.data, &number, sizeof(int));
    node.data.size = sizeof(int);

    return node;
}

bt_node* get_expr_node_int(int number)
{
    zs_expr_node expr_node;
    bt_node node;

    expr_node = expr_node_int(number);

    return create_bintree_node(make_node_data_copy(&expr_node, sizeof(zs_expr_node)));
}

bt_node_data get_special_expr_data(zs_expr_type t)
{
    zs_expr_node expr_node;
    expr_node.expr_type = t;
    expr_node.data.data = NULL;

    return make_node_data_copy(&expr_node, sizeof(zs_expr_node));
}

char* zs_expr_type_to_string(zs_expr_type expr_type)
{
    char* str;
    if(expr_type == ZS_EXPR_TYPE_ADDITION)
    {
        str = malloc(2);
        strcpy(str, "+");
    }
    else if(expr_type == ZS_EXPR_TYPE_SUBTRACTION)
    {
        str = malloc(4);
        strcpy(str, "(-)");
    }
    else if(expr_type == ZS_EXPR_TYPE_MULTIPLICATION)
    {
        str = malloc(2);
        strcpy(str, "*");
    }
    else if(expr_type == ZS_EXPR_TYPE_DIVISION)
    {
        str = malloc(2);
        strcpy(str, "/");
    }
    else if(expr_type == ZS_EXPR_TYPE_EQUAL)
    {
        str = malloc(3);
        strcpy(str, "==");
    }
    else if(expr_type == ZS_EXPR_TYPE_DIFFERENT)
    {
        str = malloc(3);
        strcpy(str, "!=");
    }
    else if(expr_type == ZS_EXPR_TYPE_SUPERIOR)
    {
        str = malloc(2);
        strcpy(str, ">");
    }
    else if(expr_type == ZS_EXPR_TYPE_INFERIOR)
    {
        str = malloc(2);
        strcpy(str, "<");
    }
    else if(expr_type == ZS_EXPR_TYPE_SUPERIOR_EQUAL)
    {
        str = malloc(3);
        strcpy(str, "<=");
    }
    else if(expr_type == ZS_EXPR_TYPE_INFERIOR_EQUAL)
    {
        str = malloc(3);
        strcpy(str, ">=");
    }
    else if(expr_type == ZS_EXPR_TYPE_VECTOR_INDEX)
    {
        str = malloc(3);
        strcpy(str, "[]");
    }
    else if(expr_type == ZS_EXPR_TYPE_NONE)
    {
        str = malloc(5);
        strcpy(str, "NONE");
    }
    else
    {
        str = NULL;
    }
    return str;
}

zs_data copy_zs_data(zs_data* data)
{
    zs_data ret;
    ret = *data;
    
    ret.data = malloc(data->size);
    if(data->data != NULL)
    {
        memcpy(ret.data, data->data, data->size);
    }
    

    return ret;
}

zs_expr_node copy_expr_node_data(zs_expr_node* data)
{
    zs_expr_node ret;

    ret = *data;
    if(ret.data.data != NULL && (ret.expr_type == ZS_EXPR_TYPE_FOLLOW_VAR_TYPE || ret.expr_type == ZS_EXPR_TYPE_VARIABLE))
    {
        ret.data = copy_zs_data(&data->data);
    }

    return ret;
}

bt_node* copy_expr_node(bt_node* node)
{
    zs_expr_node expr_node;
    bt_node* left;
    bt_node* right;

    bt_node* copy;

    if(node == NULL)
    {
        return NULL;
    }

    left = NULL;
    right = NULL;

    if(node->left != NULL)
    {
        left = copy_expr_node(node->left);
    }
    if(node->right != NULL)
    {
        right = copy_expr_node(node->right);
    }

    expr_node = copy_expr_node_data(TO_NODE_EXPR(node));

    copy = join_nodes(left, right, make_node_data_copy(&expr_node, sizeof(zs_expr_node)));

    return copy;

}

zs_data zs_interpret_expression_copy(zs_state* state, bt_node* expression, bt_node** node)
{
    bt_node* node_copy;
    zs_bool destroy_tree;

    destroy_tree = ZS_FALSE;
    node_copy = copy_expr_node(expression);

    if(node == NULL)
    {
        destroy_tree = ZS_TRUE;
    }
    else
    {
        *node = node_copy;
    }

    return zs_interpret_expression(state, node_copy, destroy_tree);
}

void print_expr_node(bt_node_data* node_data)
{
    char* data;
    zs_expr_node *expr_node;
    
    expr_node = (zs_expr_node*)node_data->data;
    if(expr_node->expr_type == ZS_EXPR_TYPE_FOLLOW_VAR_TYPE)
    {
        data = zs_get_data_as_string(&expr_node->data, 1);
        printf("%s\n", data);
        free(data);
    }
    else if(expr_node->expr_type == ZS_EXPR_TYPE_VARIABLE)
    {
        printf("<%s>\n", (char*)expr_node->data.data);
    }
    else if(expr_node->expr_type == ZS_EXPR_TYPE_FUNCTION_CALL)
    {
        printf("<()>\n");
    }
    else if(expr_node->expr_type == ZS_EXPR_TYPE_PARENTESIS)
    {
        printf("()\n");
    }
    else
    {
        data = zs_expr_type_to_string(expr_node->expr_type);
        if(data != NULL)
        {
            printf("%s\n", data);
            free(data);
        }
        else
        {
            printf("?\n");
        }
    }
    
}

zs_data resolve_expr_node(zs_state* state, zs_expr_node* expr_node, zs_var_type* type_of_var)
{
    ssize_t varindex;
    zs_data data;
    zs_data* var;
    

    if(type_of_var)
    {
        *type_of_var = ZS_TYPE_NONE;
    }

    if(expr_node->expr_type == ZS_EXPR_TYPE_VARIABLE)
    {
        varindex = zs_get_variable_index(state, (char*)expr_node->data.data);
        if(varindex == -1)
        {
            if(!is_stack_empty(state->call_stack))
            {
                
                varindex = zs_function_parameter_index(&state->functions[(*stack_top(state->call_stack, zs_function_call)).function_index], (char*)expr_node->data.data);
                if(varindex >= 0)
                {
                    var = &(*stack_top(state->call_stack, zs_function_call)).arguments[varindex];
                    
                }
                else
                {
                    zs_error("variable %s doesn't exist", expr_node->data.data);
                    data.type = ZS_TYPE_ERROR;
                    return data;
                }
            }
            else
            {
                zs_error("variable %s doesn't exist", expr_node->data.data);
                data.type = ZS_TYPE_ERROR;
                return data;
            }
            
        }
        else
        {
            var = &state->variables[varindex].data;
        }

        

        if(type_of_var)
        {
            *type_of_var = var->type;
        }

        /* copy ints and float but not other data types */
        if(var->type == ZS_TYPE_INT || var->type == ZS_TYPE_FLOAT)
        {
            data = copy_zs_data(var);
            data.isref = ZS_FALSE;
        }
        else
        {
            
            data = *var;
            data.isref = expr_node->data.isref;
        }
        

        
        expr_node->expr_type = ZS_EXPR_TYPE_FOLLOW_VAR_TYPE;
    }
    

    else if(expr_node->expr_type == ZS_EXPR_TYPE_FOLLOW_VAR_TYPE)
    {
        data = expr_node->data;
    }
    return data;
}

int zs_operate_tree_call_children(zs_state* state, bt_node* tree)
{
    if(tree->left != NULL)
    {
        if(zs_operate_tree(state, tree->left) == 0)
        {
            return 0;
        }
    }
    if(tree->right != NULL)
    {
        if(zs_operate_tree(state, tree->right) == 0)
        {
            return 0;
        }
    }
    return 1;
}

void node_int_expr_node(bt_node* node, int value)
{
    zs_create_alloc_expr_node_data(TO_NODE_EXPR(node), ZS_TYPE_INT);

    *(int*)TO_NODE_EXPR(node)->data.data = value; 
}
void node_float_expr_node(bt_node* node, float value)
{
    zs_create_alloc_expr_node_data(TO_NODE_EXPR(node), ZS_TYPE_FLOAT);

    *(float*)TO_NODE_EXPR(node)->data.data = value; 
}

int int_pow(int value, int exponent)
{
    int i;
    int product;

    product = 1;

    for(i = 0 ; i < exponent ; i++)
    {
        product *= value;
    }

    return product;
}

zs_expr_type zs_str_to_expr_type(char* str, size_t* len)
{
    if(*str == '+')
    {
        *len = 1;
        return ZS_EXPR_TYPE_ADDITION;
    }
    else if(*str == '-')
    {
        *len = 1;
        return ZS_EXPR_TYPE_SUBTRACTION;
    }   
    else if(strncmp(str, "**", 2) == 0)
    {
        *len = 2;
        return ZS_EXPR_TYPE_POWER;
    }
    else if(*str == '*')
    {
        *len = 1;
        return ZS_EXPR_TYPE_MULTIPLICATION;
    }
    else if(*str == '/')
    {
        *len = 1;
        return ZS_EXPR_TYPE_DIVISION;
    }
    else if(strncmp(str, "==", 2) == 0)
    {
        *len = 2;
        return ZS_EXPR_TYPE_EQUAL;
    } 
    else if(strncmp(str, "!=", 2) == 0)
    {
        *len = 2;
        return ZS_EXPR_TYPE_DIFFERENT;
    } 
    else if(strncmp(str, ">=", 2) == 0)
    {
        *len = 2;
        return ZS_EXPR_TYPE_SUPERIOR_EQUAL;
    } 
    else if(strncmp(str, "<=", 2) == 0)
    {
        *len = 2;
        return ZS_EXPR_TYPE_INFERIOR_EQUAL;
    } 
    else if(*str == '>')
    {
        *len = 1;
        return ZS_EXPR_TYPE_SUPERIOR;
    }
    else if(*str == '<')
    {
        *len = 1;
        return ZS_EXPR_TYPE_INFERIOR;
    }
    else
        return ZS_EXPR_TYPE_NONE;
}

void clean_expr_node_tree(bt_node* tree, zs_clean_expr_node_tree_param param, zs_state* state)
{
    char* str;
    if(tree->left != NULL)
    {
        clean_expr_node_tree(tree->left, param, state);
    }
    if(tree->right != NULL)
    {
        clean_expr_node_tree(tree->right, param, state);
    }

    
    
    if(!TO_NODE_EXPR(tree)->data.isref && TO_NODE_EXPR(tree)->data.data != NULL && (tree->parent != NULL && param == CLEAN_EXPR_NODE_TREE_ALL) && TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_FOLLOW_VAR_TYPE)
    {
        #if defined(EXPRESSION_TREE_DEBUG_ENABLED)
            str = zs_get_data_as_string(&TO_NODE_EXPR(tree)->data, 1);
            printf("DEBUG : freeing data : %s\n", str);
            free(str);
        #endif
        
        
        zs_destroy_data(&TO_NODE_EXPR(tree)->data, state);
    }
}
void destroy_expr_node_tree(bt_node* tree, zs_state* state)
{
    clean_expr_node_tree(tree, CLEAN_EXPR_NODE_TREE_ALL, state);
    free_bt_node(tree);
}


int zs_operate_tree(zs_state* state, bt_node* tree)
{
    int ret;
    void* val;
    zs_data data;
    zs_var_type var_type;
    zs_bool do_affect_variable;
    var_type = ZS_TYPE_NONE;
    ssize_t index;
    enum interpret_line_return_code_e retcode;
    char* str;
    char* currentLine;
    zs_bool comment;
    zs_bool running;

    /* operate recursivly for children */
    if(zs_operate_tree_call_children(state, tree) == 0)
    {
        return 0;
    }

    if(TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_ADDITION
    || TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_SUBTRACTION
    || TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_MULTIPLICATION
    || TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_DIVISION
    || TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_POWER)
    {
        if(cast_to_float_if_operating_with_one_int(tree) == 0)
        {
            return 0;
        }

        /* execute operation */
        val = zs_operation(TO_NODE_EXPR(tree->left)->data.data, TO_NODE_EXPR(tree->right)->data.data, TO_NODE_EXPR(tree->left)->data.type, TO_NODE_EXPR(tree)->expr_type, NULL);
        
        if(val == NULL)
        {
            return 0;
        }


        if(TO_NODE_EXPR(tree->left)->data.type == ZS_TYPE_INT && TO_NODE_EXPR(tree->right)->data.type == ZS_TYPE_INT && TO_NODE_EXPR(tree->left)->data.type && TO_NODE_EXPR(tree)->expr_type != ZS_EXPR_TYPE_POWER)
        {
            node_int_expr_node(tree, *(int*)val); 
        }
        else if(TO_NODE_EXPR(tree->left)->data.type == ZS_TYPE_FLOAT && TO_NODE_EXPR(tree->right)->data.type == ZS_TYPE_FLOAT || TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_POWER)
        {
            node_float_expr_node(tree, *(float*)val); 
        }
        else
        {
            zs_error("Bad types for operation\n");
            free(val);
            return 0;
        }
        free(val);
    }
    else if((TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_EQUAL) || (TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_DIFFERENT))
    {
        if(cast_to_float_if_operating_with_one_int(tree) == 0)
        {
            return 0;
        }
        
        if(TO_NODE_EXPR(tree->left)->data.type == TO_NODE_EXPR(tree->right)->data.type)
        {
            if(TO_NODE_EXPR(tree->left)->data.size != TO_NODE_EXPR(tree->right)->data.size)
            {
                printf("data size is not the same\n");
                ret = 0;
            }
            else
            {
                ret = !memcmp(TO_NODE_EXPR(tree->left)->data.data, TO_NODE_EXPR(tree->right)->data.data, TO_NODE_EXPR(tree->left)->data.size);
                
            }
            
            if(TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_DIFFERENT)
            {
                ret = !ret;
            }

            node_int_expr_node(tree, ret); 
        }
        else
        {
            zs_error("trying to compare data of different types");
            return 0;
        }
    }
    else if(TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_INFERIOR
    || TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_INFERIOR_EQUAL
    || TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_SUPERIOR
    || TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_SUPERIOR_EQUAL)
    {
        if(cast_to_float_if_operating_with_one_int(tree) == 0)
        {
            return 0;
        }

        ret = zs_compare_numbers(TO_NODE_EXPR(tree->left)->data.data, TO_NODE_EXPR(tree->right)->data.data, TO_NODE_EXPR(tree->left)->data.type, TO_NODE_EXPR(tree)->expr_type);

        node_int_expr_node(tree, ret); 
    }
    else if(TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_VECTOR_INDEX)
    {
        if(TO_NODE_EXPR(tree->left)->expr_type != ZS_EXPR_TYPE_FOLLOW_VAR_TYPE || (TO_NODE_EXPR(tree->left)->data.type != ZS_TYPE_VECTOR && TO_NODE_EXPR(tree->left)->data.type != ZS_TYPE_STRING))
        {
            zs_error("trying to access an index of data that's not a string variable");
            return 0;
        }
        if(TO_NODE_EXPR(tree->right)->data.type != ZS_TYPE_INT)
        {
            zs_error("trying to access an index of a vector which is not an integer");
            return 0;
        }

        if(TO_NODE_EXPR(tree->left)->data.type == ZS_TYPE_VECTOR)
        {
            if(*(int*)TO_NODE_EXPR(tree->right)->data.data > ((zs_vector*)TO_NODE_EXPR(tree->left)->data.data)->len - 1)
            {
                zs_error("array overflow");
                return 0;
            }
        }
        else if(TO_NODE_EXPR(tree->left)->data.type == ZS_TYPE_STRING)
        {
            if(*(int*)TO_NODE_EXPR(tree->right)->data.data + 1 > strlen(((char*)TO_NODE_EXPR(tree->left)->data.data)))
            {
                zs_error("string overflow");
                return 0;
            }
        }
        

        // TO_NODE_EXPR(tree)->data = [];
        if(TO_NODE_EXPR(tree->left)->data.type == ZS_TYPE_VECTOR)
        {
            TO_NODE_EXPR(tree)->data = *zs_get_element(((zs_vector*)TO_NODE_EXPR(tree->left)->data.data), *(int*)TO_NODE_EXPR(tree->right)->data.data);
        }
        else if(TO_NODE_EXPR(tree->left)->data.type == ZS_TYPE_STRING)
        {
            data.size = 2;
            data.isref = ZS_FALSE;
            data.type = ZS_TYPE_STRING;
            data.data = malloc(data.size);

            ((char*)data.data)[0] = ((char*)TO_NODE_EXPR(tree->left)->data.data)[*(int*)TO_NODE_EXPR(tree->right)->data.data];
            ((char*)data.data)[1] = '\0';

            TO_NODE_EXPR(tree)->data = data;
        }
        TO_NODE_EXPR(tree)->data.isref = ZS_TRUE;
    }
    else if(TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_FUNCTION_CALL)
    {
        
        index = zs_get_function_index(state, (char*)TO_NODE_EXPR(tree->left)->data.data);

        if(index == -1)
        {
            ret = zs_builin_function_execute(state, (char*)TO_NODE_EXPR(tree->left)->data.data, &data, &TO_NODE_EXPR(tree->right)->data, 1);
            if(ret == -1)
            {
                zs_error("function %s doesn't exist", TO_NODE_EXPR(tree->left)->data.data);
                return 0;
            }
            else if(ret == 0)
            {
                zs_error("could not call function %s", TO_NODE_EXPR(tree->left)->data.data);
                return 0;
            }
            TO_NODE_EXPR(tree)->data = data;
            return 1;
        }
        /* calling function */
        if(zs_call_function(state, index, &TO_NODE_EXPR(tree->right)->data, 1) == 0)
        {
            zs_error("couldn't call function", TO_NODE_EXPR(tree->left)->data.data);
            return 0;
        }

        

        comment = 0;
        running = 1;
        /* execute function code until return */
        while(retcode != INTERPRET_LINE_RETURN_RETURN)
        {
            retcode = zs_read_line(state, &comment, &running, 0);
            if(retcode == INTERPRET_LINE_RETURN_ERROR)
            {
                zs_error("could not call function");
                return 0;
            }
            if(retcode == INTERPRET_LINE_RETURN_ENDEF)
            {
                break;
            }
        }
        if(retcode == INTERPRET_LINE_RETURN_RETURN)
        {
            data = state->function_return;
        }
        else
        {
            data.data = NULL;
            data.type = ZS_TYPE_NONE;
            data.isref = ZS_FALSE;
        }
        TO_NODE_EXPR(tree)->data = data;

        if(tree->parent != NULL)
        {
            if(TO_NODE_EXPR(tree->parent)->expr_type == ZS_EXPR_TYPE_NONE)
            {
                TO_NODE_EXPR(tree->parent)->data = data;
            }
        }

        if(TO_NODE_EXPR(tree)->data.type == ZS_TYPE_ERROR)
        {
            return 0;
        }
    }
    else if(TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_VARIABLE)
    {
        if(tree->parent != NULL)
        {
            if(TO_NODE_EXPR(tree->parent)->expr_type == ZS_EXPR_TYPE_FUNCTION_CALL && tree->parent->left == tree)
            {
                return 1;
            }
        }
        TO_NODE_EXPR(tree)->data = resolve_expr_node(state, TO_NODE_EXPR(tree), &var_type);

        if(TO_NODE_EXPR(tree)->data.type == ZS_TYPE_ERROR)
        {
            return 0;
        }
    }
    else if(TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_PARENTESIS)
    {
        if(TO_NODE_EXPR(tree->right)->data.isref)
        {
            *TO_NODE_EXPR(tree) = *TO_NODE_EXPR(tree->right);
        }
        else
        {
            *TO_NODE_EXPR(tree) = copy_expr_node_data(TO_NODE_EXPR(tree->right));
        }
        clean_expr_node_tree(tree->right, CLEAN_EXPR_NODE_TREE_ALL, state);
        free_bt_node(tree->right);
        tree->right = NULL;

        free_bt_node(tree->left);
        tree->left = NULL;
    }
    return 1;
    
}

zs_data zs_interpret_expression(zs_state *state, bt_node* tree, zs_bool destroy_tree)
{
    zs_data data;
    char* dbgstr;

    if(tree == NULL)
    {
        zs_error("expression has error");
        data.type = ZS_TYPE_ERROR;
        return data;
    }

    if(DO_DEBUG(2))
    {
        printf("DEBUG : <zs_interpret_expression> : tree before operating\n");
        print_bintree_node(tree, print_expr_node, 0);
    }

    if(zs_operate_tree(state, tree) == 0)
    {
        data.type = ZS_TYPE_ERROR;
        free_bt_node(tree);
        return data;
    }

    if(DO_DEBUG(2))
    {   
        printf("DEBUG : <zs_interpret_expression> : tree after operating\n");
        print_bintree_node(tree, print_expr_node, 0);
    }

    data = TO_NODE_EXPR(tree)->data;
    //memcpy(&data, &TO_NODE_EXPR(tree)->data, sizeof(zs_data));
    if(DO_DEBUG(2))
    {
        printf("DEBUG : <zs_interpret_expression> : tree after operating\n");
        dbgstr = zs_get_data_as_string(&data, 1);
        printf("DEBUG : DATA : %s\n", dbgstr);
        printf("DEBUG : isref : %d\n", data.isref);
        free(dbgstr);
    }
    if(destroy_tree)
    {
        clean_expr_node_tree(tree, CLEAN_EXPR_NODE_TREE_EXCEPT_MAIN, state);
        free_bt_node(tree);
    }

    return data;
}

void zs_create_alloc_expr_node_data(zs_expr_node *data, zs_var_type type)
{
    zs_create_alloc_data(&data->data, type);
    data->expr_type = ZS_EXPR_TYPE_FOLLOW_VAR_TYPE;
    data->data.isref = ZS_FALSE;
}