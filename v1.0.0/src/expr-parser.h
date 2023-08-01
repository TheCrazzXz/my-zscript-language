#ifndef __EXPRESSION_PARSER_H__
#define __EXPRESSION_PARSER_H__

#include <stdlib.h>
#include <string.h>
#include "memory/bintree.h"
#include "zs-data.h"
#include "expr-tree.h"

#define is_ascii_digit(c) ((c >= '0') && (c <= '9')) 
#define is_ascii_int_digit(c) (is_ascii_digit(c))
#define is_character(c) ( ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')) )
#define is_variable_character(c) (is_character(c) || c == '_')

typedef enum _zs_last_addition_last_member_e
{  
    ZS_LAST_ADDITION_LAST_NONE,
    ZS_LAST_ADDITION_LAST_LEFT,
    ZS_LAST_ADDITION_LAST_RIGHT,
} zs_last_addition_last_member;

/* reads a int from an input, returns the number of digits */
size_t read_number(char* input, void** output, size_t* size, zs_var_type* type);
size_t read_varname(char* input, char** output);
bt_node create_expression_node();
size_t zs_read_next_data(char* input_expr, size_t i, zs_expr_node* expr_node, zs_bool* error);

void print_expr_node(bt_node_data* node_data);

bt_node* zs_interpret_expression_input(char* input_expr, size_t lineno);

int zs_operate_tree(zs_state* state, bt_node* tree);
#endif