#include "bintree.h"
#include <string.h>
#include <stdio.h>

bt_node_data make_node_data_copy(void* data, size_t data_size)
{
    bt_node_data node_data;

    node_data.data_size = data_size;
    node_data.data = malloc(node_data.data_size);
    memcpy(node_data.data, data, node_data.data_size);

    return node_data;
}
bt_node_data make_node_data_null()
{
    bt_node_data node_data;

    node_data.data_size = 0;
    node_data.data = NULL;

    return node_data;
}
bt_node_data make_node_data_reference(void* data, size_t data_size)
{
    bt_node_data node_data;

    node_data.data_size = data_size;
    node_data.data = data;

    return node_data;
}

bt_node_data node_data_int(int data)
{
    int* allocated_data = malloc(sizeof(int));

    *allocated_data = data;

    return make_node_data_reference(allocated_data, sizeof(int));
}
bt_node_data node_data_string(char* data)
{
    char* allocated_data = malloc(strlen(data) + 1);

    strcpy(allocated_data, data);

    return make_node_data_reference(allocated_data, strlen(data) + 1);
}

bt_node* create_bintree_node(bt_node_data node_data)
{
    bt_node* node;

    /* allocate data for the node */
    node = malloc(sizeof(bt_node));

    /* zero memory it, so that all pointers are set to NULL */
    memset(node, 0, sizeof(bt_node));

    /* copy data size */
    node->node_data = node_data;

    return node;
}

void join_nodes_core(bt_node* left, bt_node* right, bt_node* parent)
{
    /* set both left and right parent to this parent */
    if(left != NULL)
    {
        left->parent = parent;
    }
    if(right != NULL)
    {
        right->parent = parent;
    }    
    

    /* set parent children to left and right */
    parent->left = left;
    parent->right = right;
}

bt_node* join_nodes(bt_node* left, bt_node* right, bt_node_data node_data)
{
    bt_node* parent;

    if(left == NULL && right == NULL)
    {
        return create_bintree_node(node_data);
    }

    /* create new node for the parent */
    parent = create_bintree_node(node_data);

    join_nodes_core(left, right, parent);

    /* return parent */
    return parent;
}

int update_parent(bt_node* previous, bt_node* new_node)
{
    if(previous->parent != NULL)
    {
        if(previous->parent->left == previous)
        {
            previous->parent->left = new_node;
            return 1;
        }
        else if(previous->parent->right == previous)
        {
            previous->parent->right = new_node;
            return 1;
        }
    }
    return 0;
}

void direct_join_nodes(bt_node* left, bt_node* right, bt_node* parent)
{
    if(parent == NULL)
    {
        printf("Error : <direct_join_nodes in bintree.c> : parent can't be null\n");
        exit(1);
    }
    /* set both left and right parent to this parent */
    if(left != NULL)
    {
        left->parent = parent;
    }
    if(right != NULL)
    {
        right->parent = parent;
    }    

    join_nodes_core(left, right, parent);
    

    /* set parent children to left and right */
    parent->left = left;
    parent->right = right;
}

void free_bintree_node(bt_node* node)
{
    /* if there's a left then free left */
    if(node->left != NULL)
    {
        free_bt_node(node->left);
    }
    /* if there's a right then free right */
    if(node->right != NULL)
    {
        free_bt_node(node->right);
    }

    /* free this node data */
    free(node->node_data.data);
    
    /* free this node itself */
    free(node);
}

void util_print_bt_node_padding(size_t padding)
{
    size_t i;
    putc('|', stdout);
    for(i = 0 ; i < padding * 6 ; i++)
    {
        putc('-', stdout);
    }
}

void print_bt_node(bt_node* node, void (*printFunc)(bt_node_data*), size_t padding)
{
    util_print_bt_node_padding(padding);
    printFunc(&node->node_data);
    if(node->left != NULL)
    {
        printf("<%p> : ", node->left);
        print_bt_node(node->left, printFunc, padding + 1);
        
        
    }
    if(node->right != NULL)
    {
        printf("<%p> : ", node->right);
        print_bt_node(node->right, printFunc, padding + 1);
        
    }
}

void bt_print_int_func(bt_node_data* data)
{
    printf("%d\n", *(int*)data->data);
}

void bt_print_string_func(bt_node_data* data)
{
    printf("%s\n", (char*)data->data);
}