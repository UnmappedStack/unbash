#include <vector.h>
#include <stdlib.h>

Vector *new_vector(size_t num_elements, uint64_t data_size) {
    Vector *vec = (Vector*) malloc(sizeof(Vector));
    *vec = (Vector) {
        .length    = num_elements,
        .data_size = data_size,
        .data      = (uint64_t*) malloc(num_elements * data_size)
    };
}

void append_vector(Vector *vec, uint64_t new_element) {
    vec->data = (uint64_t*) realloc(vec->data, (vec->length + 1) * vec->data_size);
    vec->length++;
}

uint64_t vector_at(Vector *vec, size_t idx) {
    return vec->data[idx];
}

void vector_set(Vector *vec, size_t idx, uint64_t val) {
    vec->data[idx] = val;
}