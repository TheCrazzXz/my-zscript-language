#include "zs-functions.h"
#include "arithmetic.h"
#include "zs-data.h"
#include "expr-parser.h"
#include "expr-tree.h"
#include "zs-data.h"
#include "zs-error.h"
#include "zs-vector.h"
#include <math.h>

int zs_print(zs_state* state, char** args, size_t nArgs, zs_bool newline, size_t lineno)
{
    char* str;
    zs_data data;
    size_t i;
    char space;

    space = ' ';

    for(i = 0 ; i < nArgs ; i++)
    {
        data = zs_interpret_expression(state, zs_interpret_expression_input(args[i], lineno), ZS_TRUE);
        if(data.type == ZS_TYPE_ERROR)
        {
            zs_error("At argument %lu of print : Couldn't understand expression", i);
            return 0;
        }

        str = zs_get_data_as_string(&data, 0);

        if(i == nArgs - 1)
        {
            space = '\x00';
        }
        
        printf("%s%c", str, space);
        clean_data_if_not_reference(&data, state);

        free(str);

    }
    if(newline == ZS_TRUE)
        printf("\n");
    return 1;
}

int zs_builin_function_execute(zs_state* state, char* name, zs_data* data, zs_data* arguments, size_t nArgs)
{
    if(strcmp(name, "len") == 0)
    {
        if(nArgs != 1)
        {
            zs_error("invalid number of arguments for len");
            return 0;
        }
        zs_create_alloc_data(data, ZS_TYPE_INT);
        if(arguments[0].type == ZS_TYPE_STRING)
        {
            
            *(int*)data->data = strlen(arguments[0].data);
        }
        else if(arguments[0].type == ZS_TYPE_VECTOR)
        {
            *(int*)data->data = (int)(((zs_vector*)(arguments[0].data))->len);
        }
        else
        {
            *(int*)data->data = 1;
        }
    }
    else if(strcmp(name, "sqrt") == 0)
    {
        if(nArgs != 1)
        {
            zs_error("invalid number of arguments for sqrt");
            return 0;
        }
        zs_create_alloc_data(data, ZS_TYPE_FLOAT);
        if(arguments[0].type == ZS_TYPE_INT)
        {
            if(zs_cast_type(&arguments[0], ZS_TYPE_FLOAT) == 0)
            {
                zs_error("could not calculate square root");
                return 0;
            }
        }
        else if(arguments[0].type == ZS_TYPE_FLOAT)
        {

        }
        else
        {
            zs_error("trying to calculate square root of something that's not a number");
            return 0;
        }
        if(*(float*)arguments[0].data < 0)
        {
            zs_error("cannot take the square root of a negative number");
            return 0;
        }
        *(float*) data->data = sqrtf(*(float*)arguments[0].data);
    }
    else
    {
        zs_error("function %s not found", name);
        return -1;
    }
    return 1;
}