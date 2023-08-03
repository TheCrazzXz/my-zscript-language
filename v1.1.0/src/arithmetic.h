#ifndef __ARITHMETIC_H__
#define __ARITHMETIC_H__

#include "zs-data.h"
#include "memory/bintree.h"
#include "zs-expression.h"

int zs_compare_numbers(void* v1, void* v2, zs_var_type type, zs_expr_type expr_type);
void* zs_operation(void* v1, void* v2, zs_var_type type, zs_expr_type expr_type, void* where);
int zs_cast_type(zs_data* data, zs_var_type dsttype);
int cast_to_float_if_operating_with_one_int(bt_node* tree);

#endif