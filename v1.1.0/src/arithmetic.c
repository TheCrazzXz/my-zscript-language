#include "arithmetic.h"

#include <math.h>
#include <stdlib.h>
#include "zs-data.h"
#include "zs-expression.h"
#include "expr-tree.h"
#include "zs-error.h"

int zs_compare_numbers(void* v1, void* v2, zs_var_type type, zs_expr_type expr_type)
{
    size_t len1;
    size_t len2;

    if(expr_type == ZS_EXPR_TYPE_SUPERIOR)
    {
        if(type == ZS_TYPE_INT)
        {
            return *(int*)v1 > *(int*)v2;
        }
        else if(type == ZS_TYPE_FLOAT)
        {
            return *(float*)v1 > *(float*)v2;
        }
        
    }
    else if(expr_type == ZS_EXPR_TYPE_SUPERIOR_EQUAL)
    {
        if(type == ZS_TYPE_INT)
        {
            return *(int*)v1 >= *(int*)v2;
        }
        else if(type == ZS_TYPE_FLOAT)
        {
            return *(float*)v1 >= *(float*)v2;
        }
        
    }
    else if(expr_type == ZS_EXPR_TYPE_INFERIOR)
    {
        if(type == ZS_TYPE_INT)
        {
            return *(int*)v1 < *(int*)v2;
        }
        else if(type == ZS_TYPE_FLOAT)
        {
            return *(float*)v1 < *(float*)v2;
        }
        
    }
    else if(expr_type == ZS_EXPR_TYPE_INFERIOR_EQUAL)
    {
        if(type == ZS_TYPE_INT)
        {
            return *(int*)v1 <= *(int*)v2;
        }
        else if(type == ZS_TYPE_FLOAT)
        {
            return *(float*)v1 <= *(float*)v2;
        }
        
    }

    return 0;
}

void* zs_operation(void* v1, void* v2, zs_var_type type, zs_expr_type expr_type, void* where)
{
    size_t len1;
    size_t len2;

    void* val;
    if(where == NULL)
    {
        if(type == ZS_TYPE_INT && expr_type != ZS_EXPR_TYPE_POWER)
        {
            val = malloc(sizeof(int));
        }
        else if(type == ZS_TYPE_FLOAT || expr_type == ZS_EXPR_TYPE_POWER)
        {
            val = malloc(sizeof(float));
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        val = where;
    }
    

    if(expr_type == ZS_EXPR_TYPE_ADDITION)
    {
        if(type == ZS_TYPE_INT)
        {
            *(int*) val = *(int*)v1 + *(int*)v2;
        }
        else if(type == ZS_TYPE_FLOAT)
        {
            *(float*) val = *(float*)v1 + *(float*)v2;
        }
        
    }
    else if(expr_type == ZS_EXPR_TYPE_SUBTRACTION)
    {
        if(type == ZS_TYPE_INT)
        {
            *(int*) val = *(int*)v1 - *(int*)v2;
        }
        else if(type == ZS_TYPE_FLOAT)
        {
            *(float*) val = *(float*)v1 - *(float*)v2;
        }
    }
    else if(expr_type == ZS_EXPR_TYPE_MULTIPLICATION)
    {
        if(type == ZS_TYPE_INT)
        {
            *(int*) val = *(int*)v1 * (*(int*)v2);
        }
        else if(type == ZS_TYPE_FLOAT)
        {
            *(float*) val = *(float*)v1 * (*(float*)v2);
        }
    }
    else if(expr_type == ZS_EXPR_TYPE_DIVISION)
    {
        if(type == ZS_TYPE_INT)
        {
            *(int*) val = *(int*)v1 / (*(int*)v2);
        }
        else if(type == ZS_TYPE_FLOAT)
        {
            *(float*) val = *(float*)v1 / (*(float*)v2);
        }
    }
    else if(expr_type == ZS_EXPR_TYPE_POWER)
    {
        if(type == ZS_TYPE_INT)
        {
            *(float*) val = powf((float)(*(int*)v1), ((float)(*(int*)v2)));
        }
        else if(type == ZS_TYPE_FLOAT)
        {
            *(float*) val = powf(*(float*)v1, ((*(float*)v2)));
        }
    }
    return val;
}

int zs_cast_type(zs_data* data, zs_var_type dsttype)
{
    int ival;
    float fval;

    if(data->type == dsttype)
    {
        return 1;
    }

    if(data->type == ZS_TYPE_INT && dsttype == ZS_TYPE_FLOAT)
    {
        ival = *(int*)data->data;
        data->size = sizeof(float);
        data->data = realloc(data->data, data->size);
        *(float*)data->data = (float)ival;
    }
    else if(data->type == ZS_TYPE_FLOAT && dsttype == ZS_TYPE_INT)
    {
        fval = *(float*)data->data;
        data->size = sizeof(int);
        data->data = realloc(data->data, data->size);
        *(int*)data->data = (int)fval;
    }
    else
    {
        zs_error("Invalid conversion");
        return 0;
    }
    data->type = dsttype;
    return 1;
}  

int cast_to_float_if_operating_with_one_int(bt_node* tree)
{
    /* cast left to float if right is int or if operation is a division*/
    if(TO_NODE_EXPR(tree->left)->data.type == ZS_TYPE_INT && TO_NODE_EXPR(tree->right)->data.type == ZS_TYPE_FLOAT || TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_DIVISION)
    {
        if(zs_cast_type(&TO_NODE_EXPR(tree->left)->data, ZS_TYPE_FLOAT) == 0)
        {
            return 0;
        }
    }
    /* cast right to float if left is int or if opeartion is a division*/
    if(TO_NODE_EXPR(tree->left)->data.type == ZS_TYPE_FLOAT && TO_NODE_EXPR(tree->right)->data.type == ZS_TYPE_INT || TO_NODE_EXPR(tree)->expr_type == ZS_EXPR_TYPE_DIVISION)
    {
        if(zs_cast_type(&TO_NODE_EXPR(tree->right)->data, ZS_TYPE_FLOAT) == 0)
        {
            return 0;
        }
    }
    return 1;
}