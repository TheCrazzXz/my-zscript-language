#include "zs-data.h"
#include "memory/memutils.h"
#include "zs-vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "zs-file.h"
int zs_destroy_data(zs_data *data, zs_state* state)
{
    if(data->data != NULL)
    {
        if(data->type == ZS_TYPE_VECTOR)
        {
            zs_destroy_vector((zs_vector*)data->data, state);
        }
        else if(data->type == ZS_TYPE_FILE)
        {
            zs_closefile((zs_file*)data->data);
            free(((zs_file*)data->data)->name);
        }
        state->totaldelsize += data->size;
        free(data->data);
        data->data = NULL;
        state->ndel++;
        
        return 1;
    }
    return 0;
    
}

int clean_data_if_not_reference(zs_data* data, zs_state* state)
{
    if(data != NULL)
    {
        if(!data->isref)
        {
            return zs_destroy_data(data, state);
        }
        
    }
    return 0;
}

char* zs_get_data_as_string(zs_data* var, zs_bool as_expression)
{
    void* str;
    void* otherstr;
    size_t i;
    size_t size;
    zs_vector* vec;
    if(var->type == ZS_TYPE_STRING)
    {
        if(as_expression)
        {
            str = malloc(var->size + 2);
            strcpy(str, "\"");
            strcat(str, var->data);
            strcat(str, "\"");
        }
        else
        {
            str = malloc(var->size);
            strcpy(str, var->data);
        }
        
    }
    else if(var->type == ZS_TYPE_INT)
    {
        str = malloc(16);
        sprintf(str, "%d", *((int*)var->data));
    }
    else if(var->type == ZS_TYPE_FLOAT)
    {
        str = malloc(16);
        sprintf(str, "%f", *((float*)var->data));
    }
    else if(var->type == ZS_TYPE_VECTOR)
    {
        vec = (zs_vector*)var->data;
        size = 2;
        str = malloc(size);
        strcpy(str, "{");

        for(i = 0 ; i < vec->len ; i++)
        {
            otherstr = zs_get_data_as_string(&vec->data[i], 1);
            size += strlen(otherstr) + 1;
            str = realloc(str, size);
            strcat(str, otherstr);

            if(i < vec->len - 1)
            {
                size += 3;
                str = realloc(str, size);
                strcat(str, ", ");
            }
            
        
        }
        size += 2;
        str = realloc(str, size);
        strcat(str, "}");
    }
    else if(var->type == ZS_TYPE_NONE)
    {
        str = malloc(strlen("NONE") + 1);
        strcpy(str, "NONE");
    }
    else if(var->type == ZS_TYPE_FILE)
    {
        str = malloc(strlen(((zs_file*)var->data)->name) + 6);
        if(((zs_file*)var->data)->mode == ZS_FILE_MODE_READ)
        {
            strcpy(str, "[<");
            strcat(str, ((zs_file*)var->data)->name);
            strcat(str, "]>");
        }
        else
        {
            strcpy(str, "[>");
            strcat(str, ((zs_file*)var->data)->name);
            strcat(str, "]<");
        }
        
    }
    else
    {
        return NULL;
    }

    return str; 
}

zs_var_type zs_string_to_type(char* str)
{
    if(strcmp(str, "string") == 0)
    {
        return ZS_TYPE_STRING;
    }
    else if(strcmp(str, "int") == 0)
    {
        return ZS_TYPE_INT;
    }
    else if(strcmp(str, "float") == 0)
    {
        return ZS_TYPE_FLOAT;
    }
    else if(strcmp(str, "vector") == 0)
    {
        return ZS_TYPE_VECTOR;
    }
    else if(strcmp(str, "iofile") == 0)
    {
        return ZS_TYPE_FILE;
    }
    else if(strcmp(str, "auto") == 0)
    {
        return ZS_TYPE_AUTO;
    }
    else
    {
        return ZS_TYPE_ERROR;
    }
}

size_t zs_data_type_to_size(zs_var_type type)
{
    switch(type)
    {
        case ZS_TYPE_INT:
            return sizeof(int);
        case ZS_TYPE_FLOAT:
            return sizeof(float);
        case ZS_TYPE_VECTOR:
            return sizeof(zs_vector);
        case ZS_TYPE_FILE:
            return sizeof(zs_file);
        default:
            return 0;
    }
}
void zs_create_alloc_data(zs_data* data, zs_var_type type)
{
    size_t size;

    size = zs_data_type_to_size(type);
    if(size > 0)
    {
        zs_set_size_and_alloc(*data, size);
    }
    if(type == ZS_TYPE_NONE)
    {
        data->data = NULL;
    }
    if(type == ZS_TYPE_VECTOR)
    {
        *(zs_vector*)data->data = zs_create_vector();
    }
    data->type = type;
}

