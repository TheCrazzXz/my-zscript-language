#include "zs-functions.h"
#include "args-parser.h"
#include "arithmetic.h"
#include "memory/memutils.h"
#include "zs-data.h"
#include "expr-parser.h"
#include "expr-tree.h"
#include "zs-data.h"
#include "zs-error.h"
#include "zs-file.h"
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
    zs_bool error;
    size_t size;
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
            zs_error("invalid number of arguments for sqrt (1 required)");
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
    else if(strcmp(name, "concatstr") == 0)
    {
        if(nArgs != 2)
        {
            zs_error("invalid number of arguments for concatstr (2 required)");
            return 0;
        }
        zs_create_alloc_data(data, ZS_TYPE_STRING);
        if(arguments[0].type != ZS_TYPE_STRING || arguments[1].type != ZS_TYPE_STRING)
        {
            zs_error("arguments should be strings");
            return 0;
        }
        zs_set_size_and_alloc(*data, arguments[0].size + arguments[1].size - 1);
        strcpy(data->data, arguments[0].data);
        strcat(data->data, arguments[1].data);
    }
    else if(strcmp(name, "randint") == 0)
    {
        if(nArgs != 2)
        {
            zs_error("invalid number of arguments for randint (2 required)");
            return 0;
        }
        zs_create_alloc_data(data, ZS_TYPE_INT);
        if(arguments[0].type != ZS_TYPE_INT || arguments[1].type != ZS_TYPE_INT)
        {
            zs_error("two arguments should be ints");
            return 0;
        }
        *(int*) data->data = randint(*(int*)arguments[0].data, *(int*)arguments[1].data);
    }
    else if(strcmp(name, "char") == 0)
    {
        if(nArgs != 1)
        {
            zs_error("invalid number of arguments for char (1 required)");
            return 0;
        }
        zs_create_alloc_data(data, ZS_TYPE_STRING);
        data->size = 2;
        data->data = malloc(data->size);
        
        data->data = alloc_and_copy_string("x");
        if(arguments[0].type != ZS_TYPE_INT)
        {
            zs_error("argument should be int");
            return 0;
        }
        ((char*) data->data)[0] = (char)(*(int*)arguments[0].data);
    }
    else if(strcmp(name, "int") == 0)
    {
        if(nArgs != 1)
        {
            zs_error("invalid number of arguments for int (1 required)");
            return 0;
        }
        
        zs_create_alloc_data(data, ZS_TYPE_INT);

        if(arguments[0].type == ZS_TYPE_FLOAT)
        {
            *((int*) data->data) = (int)(*(float*)arguments[0].data);
        }
        else if(arguments[0].type == ZS_TYPE_STRING)
        {   
            *((int*) data->data) = atoi(arguments[0].data);
        }
        else
        {
            zs_error("argument should be either string of float");
            return 0;
        }
    }
    else if(strcmp(name, "openfile") == 0)
    {
        if(nArgs != 2)
        {
            zs_error("invalid number of arguments for openfile (2 required)");
            return 0;
        }
        zs_create_alloc_data(data, ZS_TYPE_FILE);
        *(zs_file*)data->data = zs_openfile(arguments[0].data, arguments[1].data, &error);
        if(error)
        {
            zs_error("could not open file");
            return 0;
        }
    }
    else if(strcmp(name, "writefile") == 0)
    {
        if(nArgs != 2)
        {
            zs_error("invalid number of arguments for writefile (2 required)");
            return 0;
        }
        
        if(arguments[0].type != ZS_TYPE_FILE)
        {
            zs_error("first argument should be the file");
            return 0;
        }
        if(((zs_file*)arguments[0].data)->mode != ZS_FILE_MODE_WRITE)
        {
            zs_error("the file is not opened to write");
            return 0;
        }
        if(((zs_file*)arguments[0].data)->fp == NULL)
        {
            zs_error("trying to write on closed file");
            return 0;
        }
        zs_create_alloc_data(data, ZS_TYPE_NONE);
        if(arguments[1].type == ZS_TYPE_STRING)
        {
            
            fwrite(arguments[1].data, arguments[1].size - 1, 1, ((zs_file*)arguments[0].data)->fp);
        }
        else
        {
            zs_error("couldn't write data to file");
            return 0;
        }
    }
    else if(strcmp(name, "readfile") == 0)
    {
        if(nArgs < 1)
        {
            zs_error("invalid number of arguments for readfile (1 required, 1 optionnal)");
            return 0;
        }
        if(arguments[0].type != ZS_TYPE_FILE)
        {
            zs_error("argument should be the file");
            return 0;
        }
        if(((zs_file*)arguments[0].data)->fp == NULL)
        {
            zs_error("trying to read from closed file");
            return 0;
        }
        if(feof(((zs_file*)arguments[0].data)->fp))
        {
            zs_error("End of file");
            return 0;
        }
        if(((zs_file*)arguments[0].data)->mode != ZS_FILE_MODE_READ)
        {
            zs_error("the file is not opened to read");
            return 0;
        }
        

        if(nArgs == 1)
        {
            size = get_file_size(((zs_file*)arguments[0].data)->fp);
        }
        else if(nArgs == 2)
        {
            if(arguments[0].type != ZS_TYPE_FILE || arguments[1].type != ZS_TYPE_INT)
            {
                zs_error("first argument should be the file and second argument should be int");
            }
            size = *(int*)arguments[1].data;
        }
        else
        {
            zs_error("too much arguments for readfile (1 required, 1 optionnal)");
        }
        
        zs_create_alloc_data(data, ZS_TYPE_STRING);
        zs_set_size_and_alloc(*data, size);      
        
        fread(data->data, size, 1, ((zs_file*)arguments[0].data)->fp);
    }
    else if(strcmp(name, "closefile") == 0)
    {
        if(nArgs != 1)
        {
            zs_error("invalid number of arguments for readfile (1 required)");
            return 0;
        }
        if(arguments[0].type != ZS_TYPE_FILE)
        {
            zs_error("argument should be the file");
            return 0;
        }
        if(((zs_file*)arguments[0].data)->fp == NULL)
        {
            zs_error("trying to close at closed file");
            return 0;
        }
        zs_create_alloc_data(data, ZS_TYPE_NONE);
        zs_closefile(((zs_file*)arguments[0].data));
    }
    else
    {
        zs_error("function %s not found", name);
        return -1;
    }
    return 1;
}