#ifndef __ZS_EXPRESSION_H__
#define __ZS_EXPRESSION_H__

#include "zs-data.h"

typedef enum zs_expression_type_e
{
    /* no expression */
    ZS_EXPR_TYPE_NONE, 

    /* type */
    ZS_EXPR_TYPE_FOLLOW_VAR_TYPE,
    ZS_EXPR_TYPE_VARIABLE,

    /* operations */
    ZS_EXPR_TYPE_ADDITION, /* (+) */
    ZS_EXPR_TYPE_SUBTRACTION, /* (-) */
    ZS_EXPR_TYPE_MULTIPLICATION, /* (*) */
    ZS_EXPR_TYPE_DIVISION, /* (/) */
    ZS_EXPR_TYPE_POWER,

    /* comparaisons */
    ZS_EXPR_TYPE_EQUAL, /* == */
    ZS_EXPR_TYPE_DIFFERENT, /* != */ 
    ZS_EXPR_TYPE_INFERIOR, /* < */
    ZS_EXPR_TYPE_SUPERIOR, /* > */
    ZS_EXPR_TYPE_INFERIOR_EQUAL, /* <= */
    ZS_EXPR_TYPE_SUPERIOR_EQUAL, /* >= */

    /* vectors */
    ZS_EXPR_TYPE_VECTOR_INDEX, /* [] */

    /* functions */
    ZS_EXPR_TYPE_FUNCTION_CALL, /* <function>(<param>) */

    /* parentesis */
    ZS_EXPR_TYPE_PARENTESIS /* (<expression>) */
} zs_expr_type;


typedef struct zs_expression_t
{
    size_t size;
    void* data;
    zs_var_type var_type;
    zs_expr_type expr_type;
} zs_expr;

#endif