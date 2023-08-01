#ifndef __BIN_TREE_H__
#define __BIN_TREE_H__

#include <stdlib.h>

typedef struct __bt_node_data_t
{
    size_t data_size;
    void* data;
} bt_node_data;

/* creates a new node data by copying data to allocated memory */
bt_node_data make_node_data_copy(void* data, size_t data_size);
/* creates new node data by setting the data pointer to data, data has to be mallock'd memory */
bt_node_data make_node_data_reference(void* data, size_t data_size);

/* special cases for some data types */
bt_node_data node_data_int(int data);
bt_node_data node_data_string(char* data);

#define nodedata_cpy make_node_data_copy 
#define nodedata_ref make_node_data_reference
typedef struct __bt_node_t
{
    bt_node_data node_data;

    struct __bt_node_t* parent;
    struct __bt_node_t* right;
    struct __bt_node_t* left;
} bt_node;

/* creates new bintree node copying data_size bytes from data to it (copying data to its data)*/
bt_node* create_bintree_node(bt_node_data node_data);

int update_parent(bt_node* previous, bt_node* new_node);

/* joins two tree and returns parent copying data to parent's data */
bt_node* join_nodes(bt_node* left, bt_node* right, bt_node_data node_data);

void direct_join_nodes(bt_node* left, bt_node* right, bt_node* parent);

/* free all the children of a node and free the data and the node itself */
void free_bintree_node(bt_node* node);

void print_bintree_node(bt_node* node, void (*printFunc)(bt_node_data*), size_t padding);

void bt_print_int_func(bt_node_data* data);
void bt_print_string_func(bt_node_data* data);

#define create_bt_node create_bintree_node 
#define free_bt_node free_bintree_node  
#define print_bt_node print_bintree_node 

#endif