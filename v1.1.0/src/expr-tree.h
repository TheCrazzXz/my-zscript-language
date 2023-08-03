#ifndef __EXPRESSION_TREE_H__
#define __EXPRESSION_TREE_H__

#include "zs-data.h"
#include "zs-expression.h"
#include "memory/bintree.h"

#define TO_NODE_EXPR(node) ((zs_expr_node*)node->node_data.data)

typedef struct zs_expr_node_t
{
    zs_expr_type expr_type;
    zs_data data;

} zs_expr_node;

zs_expr_node expr_node_int(int number);

bt_node* get_expr_node_int(int number);

bt_node_data get_special_expr_data(zs_expr_type t);

zs_expr_type zs_str_to_expr_type(char* c, size_t* len);

bt_node* copy_expr_node(bt_node* node);

void print_expr_node(bt_node_data* node_data);
zs_data resolve_expr_node(zs_state* state, zs_expr_node* expr_node, zs_var_type* type_of_var);
int zs_operate_tree(zs_state* state, bt_node* tree);
zs_data zs_interpret_expression(zs_state *state, bt_node* tree, zs_bool destroy_tree);
zs_data zs_interpret_expression_copy(zs_state* state, bt_node* expression, bt_node** node);

typedef enum __zs_clean_expr_node_tree_param_e
{
    CLEAN_EXPR_NODE_TREE_ALL,
    CLEAN_EXPR_NODE_TREE_EXCEPT_MAIN
} zs_clean_expr_node_tree_param;

void clean_expr_node_tree(bt_node* tree, zs_clean_expr_node_tree_param param, zs_state* state);
void destroy_expr_node_tree(bt_node* tree, zs_state* state);

void zs_create_alloc_expr_node_data(zs_expr_node *data, zs_var_type type);

#endif