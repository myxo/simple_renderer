#pragma once

#include "geometry.h"

typedef struct matrix_{
    int n, m;
    float **array;
} matrix;


void matrix_initialize(matrix *mat, int n, int m);
void matrix_delete(matrix *mat);

void matrix_copy(matrix *from, matrix *to);

void matrix_sum(matrix *a, matrix *b, matrix *res);
void matrix_sub(matrix *a, matrix *b, matrix *res);
void matrix_product_scalar(matrix *a, float alpha, matrix *res);
void matrix_product(matrix *a, matrix *b, matrix *res);
matrix* matrix_list_product(matrix **list, int n, matrix *result);

void matrix_identity(matrix *a);

void matrix_set(matrix *a, float *array, int length);
void matrix_print(matrix *a);
void matrix_embed_from_point(matrix *a, vec3D p);

void matrix4x4_inverse(matrix *a, matrix *res);
