#ifndef __ZSCRIPT_INTERPRETER_H__
#define __ZSCRIPT_INTERPRETER_H__

#include <stdio.h>
#include "zs-data.h"
#include "memory/bintree.h"
typedef enum interpret_line_return_code_e
{
    INTERPRET_LINE_RETURN_NOTHING,
    INTERPRET_LINE_RETURN_SUCCESS,
    INTERPRET_LINE_RETURN_RETURN,
    INTERPRET_LINE_RETURN_ENDEF,
    INTERPRET_LINE_RETURN_ERROR,
    INTERPRET_LINE_RETURN_EXIT
} interprl_rcode;

interprl_rcode zs_read_line(zs_state* state, zs_bool* comment, zs_bool* running, size_t lineno);
interprl_rcode zs_interpret_line(char* line, zs_state* state, size_t lineno);

void zs_intepret_code(FILE* fp);

#endif