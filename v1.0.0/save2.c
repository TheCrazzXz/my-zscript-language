if(currentNode != NULL)
                {
                    
                    tempnode = join_nodes(create_expr_node(&temp), create_bintree_node(make_node_data_copy(NULL, 0)), get_special_expr_data(ZS_EXPR_TYPE_FUNCTION_CALL));
                    direct_join_nodes(copy_expr_node(currentNode), tempnode, currentNode);
                    currentNode->node_data = get_special_expr_data(expr_parentesis_stack_top(parentesis)->op_pending);
                    expr_par.prev_node = (currentNode);
                    currentNode = ((currentNode)->right)->right;
                }
                else
                {
                    
                    expr_par.one_elem = ZS_TRUE;
                    currentNode = join_nodes(
                            create_expr_node(&temp),
                            create_bintree_node(make_node_data_copy(NULL, 0)),
                            get_special_expr_data(ZS_EXPR_TYPE_FUNCTION_CALL)
                        );
                    expr_par.prev_node = (currentNode);
                    tree = currentNode;
                    currentNode = ((currentNode)->right);
                    
                }
                print_bt_node(currentNode, print_expr_node, 0);
                
                expr_par.op_pending = ZS_EXPR_TYPE_NONE;
                

                stack_push(&parentesis, get_expr_parentesis_struct_stack_data(expr_par));

                
                
                i++;