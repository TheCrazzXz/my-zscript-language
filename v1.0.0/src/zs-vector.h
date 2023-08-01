#ifndef __ZS_VECTOR_H__
#define __ZS_VECTOR_H__

#include <stdlib.h>
#include "zs-data.h"

typedef struct __zs_vector_t
{
    size_t len; /* number of elements */
    zs_data* data; /* data of the list */
} zs_vector;

zs_vector zs_create_vector();
void zs_add_element(zs_vector* vector, zs_data data, zs_state* state);
void zs_destroy_vector(zs_vector* vector, zs_state* state);
zs_data* zs_get_element(zs_vector* vector, size_t index);
void zs_set_element(zs_vector* vector, size_t index, zs_data data, zs_state* state);
#endif