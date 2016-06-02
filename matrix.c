#include "matrix.h"

#include <stdlib.h>
#include <stdio.h>

void matrix_initialize(matrix *mat, int n, int m){
    mat->n = n;
    mat->m = m;

    mat->array = (float **) malloc (sizeof(float *) * n);
    for (int i = 0; i < n; i++){
        mat->array[i] = (float *) malloc(sizeof(float) * m);
    }
}

void matrix_delete(matrix *mat){
    for (int i = 0; i < mat->n; i++){
        free(mat->array[i]);
    }    
    free(mat->array);
}


void matrix_sum(matrix *a, matrix *b, matrix *res){
    if (a->n != b->n || a->m != b->m){
        printf("ERROR in matrix sum\n");
    }
    matrix_initialize(res, a->n, a->m);

    for (int i = 0; i < a->n; i++){
        for (int j = 0; j < a->m; j++){
            res->array[i][j] = a->array[i][j] + b->array[i][j];
        }
    }
}


void matrix_sub(matrix *a, matrix *b, matrix *res){
    if (a->n != b->n || a->m != b->m){
        printf("ERROR in matrix sub\n");
    }
    matrix_initialize(res, a->n, a->m);

    for (int i = 0; i < a->n; i++){
        for (int j = 0; j < a->m; j++){
            res->array[i][j] = a->array[i][j] - b->array[i][j];
        }
    }
}


void matrix_product_scalar(matrix *a, float alpha, matrix *res){
    matrix_initialize(res, a->n, a->m);

    for (int i = 0; i < a->n; i++){
        for (int j = 0; j < a->m; j++){
            res->array[i][j] = a->array[i][j] * alpha;
        }
    }
}


void matrix_product(matrix *a, matrix *b, matrix *res){
    if (a->m != b->n){
        printf("ERROR in matrix product\n");
    }

    matrix_initialize(res, a->n, b->m);

    for (int i = 0; i < a->n; i++){
        for (int j = 0; j < b->m; j++){
            float s = 0;
            for (int k = 0; k < a->m; k++){
                s += a->array[i][k] * b->array[k][j];
            }
            res->array[i][j] = s;
        }
    }
}


float matrix_get(matrix *a, int i, int j){
    return a->array[i][j];
}

void matrix_set(matrix *a, float *array, int length){
    if (a->n * a->m != length){
        printf("ERROR in matrix set\n");
    }

    int k = 0;
    for (int i = 0; i < a->n; i++){
        for (int j = 0; j < a->m; j++){
            a->array[i][j] = array[k++];
        }
    }
}


void matrix_print(matrix *a){
    for (int i = 0; i < a->n; i++){
        for (int j = 0; j < a->m; j++){
            printf("%8.2f ", a->array[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}