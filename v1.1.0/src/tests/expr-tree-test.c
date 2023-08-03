#include <stdio.h>
#include "../expr-tree.h"
#include <string.h>

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

    return make_node_data_copy(&expr_node, sizeof(zs_expr_node));
}

int main(void)
{
    zs_state state;
    bt_node *expr_node;
    zs_data data;

    state.nVars = 0;
    state.variables = NULL;

    /* ((5 / 5) - 7 * 8) * (38 + 5) * (10 / 2) */
    expr_node = join_nodes(
        /* ((5 / 5) - 7 * 8) * (38 + 5) */
        join_nodes(
            /* ((5 / 5) - 7 * 8) */
            join_nodes(
                /* 5 / 5 */
                join_nodes(get_expr_node_int(5), get_expr_node_int(5), get_special_expr_data(ZS_EXPR_TYPE_DIVISION)),

                /* 7 * 8 */
                join_nodes(get_expr_node_int(7), get_expr_node_int(8), get_special_expr_data(ZS_EXPR_TYPE_MULTIPLICATION)),
                get_special_expr_data(ZS_EXPR_TYPE_SUBTRACTION)),

            /* 38 + 5 */ 
            join_nodes(get_expr_node_int(38), get_expr_node_int(5), get_special_expr_data(ZS_EXPR_TYPE_ADDITION)),
        get_special_expr_data(ZS_EXPR_TYPE_MULTIPLICATION)), 

        /* 10 / 2 */
        join_nodes(get_expr_node_int(10), get_expr_node_int(2), get_special_expr_data(ZS_EXPR_TYPE_DIVISION)), 
        get_special_expr_data(ZS_EXPR_TYPE_MULTIPLICATION)
    );

    data = zs_interpret_expression(&state, expr_node, ZS_TRUE);
    printf("data : %s\n", zs_get_data_as_string(&data, 1));
}
