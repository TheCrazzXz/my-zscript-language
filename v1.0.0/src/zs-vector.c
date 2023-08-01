#include "zs-vector.h"
#include "memory/memutils.h"
#include "zs-data.h"
#include <string.h>

zs_vector zs_create_vector()
{
    zs_vector ret;
    memset(&ret, 0, sizeof(zs_vector));
    return ret;
}

void zs_set_element(zs_vector* vector, size_t index, zs_data data, zs_state* state)
{
    size_t i;
    size_t nelems_to_add_minus_one;
    zs_data new_data;
    if(index + 1 > vector->len)
    {
        nelems_to_add_minus_one = index - vector->len;
        for(i = 0 ; i < nelems_to_add_minus_one ; i++)
        {
            zs_create_alloc_data(&new_data, ZS_TYPE_NONE);
            zs_add_element(vector, new_data, state);
        }
        ALLOCATION_FOR_NEW_ELEM_VECTOR(vector->data, vector->len, sizeof(zs_data));
    }
    else
    {
        clean_data_if_not_reference(vector->data[index].data, state);
    }
    vector->data[index] = data;
}

zs_data* zs_get_element(zs_vector* vector, size_t index)
{
    return &vector->data[index];
}

void zs_add_element(zs_vector* vector, zs_data data, zs_state* state)
{
    ALLOCATION_FOR_NEW_ELEM_VECTOR(vector->data, vector->len, sizeof(zs_data));
    vector->data[vector->len - 1].data = NULL;
    zs_set_element(vector, vector->len - 1, data, state);
}

void zs_destroy_vector(zs_vector* vector, zs_state* state)
{
    size_t i;

    for(i = 0 ; i < vector->len ; i++)
    {
        if(vector->data[i].data != NULL)
        {
            clean_data_if_not_reference(&vector->data[i], state);
        }
    }
}