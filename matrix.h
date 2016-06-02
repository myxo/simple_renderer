#pragma once


typedef struct matrix_{
    int n, m;
    float **array;
} matrix;


void matrix_initialize(matrix *mat, int n, int m);
void matrix_delete(matrix *mat);

void matrix_sum(matrix *a, matrix *b, matrix *res);
void matrix_sub(matrix *a, matrix *b, matrix *res);
void matrix_product_scalar(matrix *a, float alpha, matrix *res);
void matrix_product(matrix *a, matrix *b, matrix *res);

void matrix_set(matrix *a, float *array, int length);
void matrix_print(matrix *a);
