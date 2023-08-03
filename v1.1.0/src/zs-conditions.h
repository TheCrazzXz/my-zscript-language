#ifndef __ZS_CONDITIONS_H__
#define __ZS_CONDITIONS_H__

#include "zs-data.h"
#include "memory/bintree.h"
typedef enum __zs_conditionnal_structure_type_e
{
    ZS_CONDTIONNAL_STRUCTURE_IF,
    ZS_CONDTIONNAL_STRUCTURE_ELSE,
} zs_conditionnal_structure_type;


typedef struct __zs_conditionnal_structure_t
{
    zs_bool shadow; /* statement that should not be executed */
    zs_bool value; /* was if = 1 or 0 */
    zs_conditionnal_structure_type type; /* if or else */
} zs_conditionnal_structure;

int zs_new_conditionnal_structure(zs_state* state, bt_node* expression_node);
int zs_check_shadow_if(zs_state* state);
zs_bool should_condition_code_be_executed(zs_state* state);
int zs_check_condition_code_to_be_executed(zs_state* state);
int zs_new_else_statement(zs_state* state);
int zs_check_end_conditionnal_statement(zs_state* state);

#endif