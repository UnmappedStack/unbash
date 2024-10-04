#pragma once
#include <stddef.h>
#include <stdint.h>

typedef struct {
    size_t   length;
    size_t   data_size;
    uint64_t *data;
} Vector;

Vector *new_vector(size_t num_elements, uint64_t data_size);
void append_vector(Vector *vec, uint64_t new_element);
uint64_t vector_at(Vector *vec, size_t idx);
void vector_set(Vector *vec, size_t idx, uint64_t val);