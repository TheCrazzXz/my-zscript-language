#ifndef __ZS_LOOP_H__
#define __ZS_LOOP_H__

#include "zs-data.h"
#include "memory/bintree.h"

/* loop structure */
typedef struct __zs_loop_structure_t
{
    zs_bool value; /* value at first iteration */
    zs_bool shadow; /* is code that is inside not executed */
    zs_bool cracking_loop; /* force exit loop */
    size_t loop_content_start_off; /* off from fp SEEK_SET to loop from */
    bt_node* loop_expression; /* expression to test for loop */
    char* iteration_code;
} zs_loop_structure;

int zs_new_loop(zs_state* state, bt_node* expression_node, char* it_code);
zs_bool should_loop_code_be_executed(zs_state* state);
int zs_check_loop_code_to_be_executed(zs_state* state);

void zs_check_execute_iteration_code(zs_state* state);

void zs_end_current_loop(zs_state* state);

typedef enum __zs_endloop_state_e
{
    ZS_ENDLOOP_STATE_ERROR,
    ZS_ENDLOOP_STATE_LOOP_ENDED,
    ZS_ENDLOOP_STATE_LOOP_AGAIN
} zs_endloop_state;

zs_endloop_state zs_check_endloop(zs_state* state);
int zs_crack_loop(zs_state* state);
int zs_reiterate_loop(zs_state* state);
#endif