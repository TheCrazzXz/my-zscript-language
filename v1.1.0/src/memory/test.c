#include <stdio.h>
#include "bintree.h"

int main(void)
{
    bt_node* tree;
    size_t num;

    num = 55;


    tree = join_nodes(
        join_nodes(
            create_bt_node(node_data_string("hello")), 
            join_nodes(
                create_bt_node(node_data_string("hi")),
                create_bt_node(node_data_string("what's up")),
                node_data_string("hello !")
            ), node_data_string("bonjour")),

        join_nodes(create_bt_node(node_data_string("aurevoir")), create_bt_node(node_data_string("hey")), node_data_string("hi")),
        node_data_string("bonjour")
    );

    print_bt_node(tree, bt_print_string_func, 0);

    free_bt_node(tree);
}