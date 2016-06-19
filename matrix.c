#include "matrix.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//TODO matrix function lead memory leak. Fix

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


void matrix_copy(matrix *from, matrix *to){
    matrix_initialize(to, from->n, from->m);

    for (int i = 0; i < to->n; i++){
        for (int j = 0; j < to->m; j++){
            to->array[i][j] = from->array[i][j];
        }
    }
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

matrix* matrix_list_product(matrix **list, int n, matrix *result){
    matrix *prev, *res;

    res = (matrix *)malloc(sizeof(matrix));
    matrix_initialize(res, list[0]->n, list[1]->m);
    matrix_product(list[0], list[1], res);
    
    
    for (int i = 2; i < n; i++){
        prev = res;
        res = (matrix *)malloc(sizeof(matrix));
        matrix_initialize(res, prev->n, list[i]->m);

        matrix_product(prev, list[i], res);

        matrix_delete(prev);
        free(prev);
    }

    matrix_copy(res, result);
    return res;
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
            printf("%8.4f ", a->array[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}


void matrix_identity(matrix *a){
    for (int i = 0; i < a->n; i++){
        for (int j = 0; j < a->m; j++){
            a->array[i][j] = (i == j)? 1: 0;            
        }
    }
}


void matrix_embed_from_point(matrix *a, vec3D p){
    matrix_initialize(a, 4, 1);
    a->array[0][0] = p.x;
    a->array[1][0] = p.y;
    a->array[2][0] = p.z;
    a->array[3][0] = 1.0;
}


// bool gluInvertMatrix(const double m[16], double invOut[16])
// {
//     double inv[16], det;
//     int i;

//     inv[0] = m[5]  * m[10] * m[15] - 
//              m[5]  * m[11] * m[14] - 
//              m[9]  * m[6]  * m[15] + 
//              m[9]  * m[7]  * m[14] +
//              m[13] * m[6]  * m[11] - 
//              m[13] * m[7]  * m[10];

//     inv[4] = -m[4]  * m[10] * m[15] + 
//               m[4]  * m[11] * m[14] + 
//               m[8]  * m[6]  * m[15] - 
//               m[8]  * m[7]  * m[14] - 
//               m[12] * m[6]  * m[11] + 
//               m[12] * m[7]  * m[10];

//     inv[8] = m[4]  * m[9] * m[15] - 
//              m[4]  * m[11] * m[13] - 
//              m[8]  * m[5] * m[15] + 
//              m[8]  * m[7] * m[13] + 
//              m[12] * m[5] * m[11] - 
//              m[12] * m[7] * m[9];

//     inv[12] = -m[4]  * m[9] * m[14] + 
//                m[4]  * m[10] * m[13] +
//                m[8]  * m[5] * m[14] - 
//                m[8]  * m[6] * m[13] - 
//                m[12] * m[5] * m[10] + 
//                m[12] * m[6] * m[9];

//     inv[1] = -m[1]  * m[10] * m[15] + 
//               m[1]  * m[11] * m[14] + 
//               m[9]  * m[2] * m[15] - 
//               m[9]  * m[3] * m[14] - 
//               m[13] * m[2] * m[11] + 
//               m[13] * m[3] * m[10];

//     inv[5] = m[0]  * m[10] * m[15] - 
//              m[0]  * m[11] * m[14] - 
//              m[8]  * m[2] * m[15] + 
//              m[8]  * m[3] * m[14] + 
//              m[12] * m[2] * m[11] - 
//              m[12] * m[3] * m[10];

//     inv[9] = -m[0]  * m[9] * m[15] + 
//               m[0]  * m[11] * m[13] + 
//               m[8]  * m[1] * m[15] - 
//               m[8]  * m[3] * m[13] - 
//               m[12] * m[1] * m[11] + 
//               m[12] * m[3] * m[9];

//     inv[13] = m[0]  * m[9] * m[14] - 
//               m[0]  * m[10] * m[13] - 
//               m[8]  * m[1] * m[14] + 
//               m[8]  * m[2] * m[13] + 
//               m[12] * m[1] * m[10] - 
//               m[12] * m[2] * m[9];

//     inv[2] = m[1]  * m[6] * m[15] - 
//              m[1]  * m[7] * m[14] - 
//              m[5]  * m[2] * m[15] + 
//              m[5]  * m[3] * m[14] + 
//              m[13] * m[2] * m[7] - 
//              m[13] * m[3] * m[6];

//     inv[6] = -m[0]  * m[6] * m[15] + 
//               m[0]  * m[7] * m[14] + 
//               m[4]  * m[2] * m[15] - 
//               m[4]  * m[3] * m[14] - 
//               m[12] * m[2] * m[7] + 
//               m[12] * m[3] * m[6];

//     inv[10] = m[0]  * m[5] * m[15] - 
//               m[0]  * m[7] * m[13] - 
//               m[4]  * m[1] * m[15] + 
//               m[4]  * m[3] * m[13] + 
//               m[12] * m[1] * m[7] - 
//               m[12] * m[3] * m[5];

//     inv[14] = -m[0]  * m[5] * m[14] + 
//                m[0]  * m[6] * m[13] + 
//                m[4]  * m[1] * m[14] - 
//                m[4]  * m[2] * m[13] - 
//                m[12] * m[1] * m[6] + 
//                m[12] * m[2] * m[5];

//     inv[3] = -m[1] * m[6] * m[11] + 
//               m[1] * m[7] * m[10] + 
//               m[5] * m[2] * m[11] - 
//               m[5] * m[3] * m[10] - 
//               m[9] * m[2] * m[7] + 
//               m[9] * m[3] * m[6];

//     inv[7] = m[0] * m[6] * m[11] - 
//              m[0] * m[7] * m[10] - 
//              m[4] * m[2] * m[11] + 
//              m[4] * m[3] * m[10] + 
//              m[8] * m[2] * m[7] - 
//              m[8] * m[3] * m[6];

//     inv[11] = -m[0] * m[5] * m[11] + 
//                m[0] * m[7] * m[9] + 
//                m[4] * m[1] * m[11] - 
//                m[4] * m[3] * m[9] - 
//                m[8] * m[1] * m[7] + 
//                m[8] * m[3] * m[5];

//     inv[15] = m[0] * m[5] * m[10] - 
//               m[0] * m[6] * m[9] - 
//               m[4] * m[1] * m[10] + 
//               m[4] * m[2] * m[9] + 
//               m[8] * m[1] * m[6] - 
//               m[8] * m[2] * m[5];

//     det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

//     if (det == 0)
//         return false;

//     det = 1.0 / det;

//     for (i = 0; i < 16; i++)
//         invOut[i] = inv[i] * det;

//     return true;
// }


void Invert2(float *mat, float *dst)
{
 float tmp[12]; /* temp array for pairs */
 float src[16]; /* array of transpose source matrix */
 float det; /* determinant */
 /* transpose matrix */
 for (int i = 0; i < 4; i++) {
 src[i] = mat[i*4];
 src[i + 4] = mat[i*4 + 1];
 src[i + 8] = mat[i*4 + 2];
 src[i + 12] = mat[i*4 + 3];
 }
 /* calculate pairs for first 8 elements (cofactors) */
 tmp[0] = src[10] * src[15];
 tmp[1] = src[11] * src[14];
 tmp[2] = src[9] * src[15];
 tmp[3] = src[11] * src[13];
 tmp[4] = src[9] * src[14];
 tmp[5] = src[10] * src[13];
 tmp[6] = src[8] * src[15];
 tmp[7] = src[11] * src[12];
 tmp[8] = src[8] * src[14];
 tmp[9] = src[10] * src[12];
 tmp[10] = src[8] * src[13];
 tmp[11] = src[9] * src[12];
 /* calculate first 8 elements (cofactors) */
 dst[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
 dst[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
 dst[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
 dst[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
 dst[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
 dst[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
 dst[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
 dst[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
 dst[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
 dst[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
 dst[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
 dst[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
 dst[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
 dst[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
 dst[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
 dst[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];
 /* calculate pairs for second 8 elements (cofactors) */
 tmp[0] = src[2]*src[7];
 tmp[1] = src[3]*src[6];
 tmp[2] = src[1]*src[7];
 tmp[3] = src[3]*src[5];
 tmp[4] = src[1]*src[6];
 tmp[5] = src[2]*src[5];
 tmp[6] = src[0]*src[7];
 tmp[7] = src[3]*src[4];
 tmp[8] = src[0]*src[6];
 tmp[9] = src[2]*src[4];
 tmp[10] = src[0]*src[5];
 tmp[11] = src[1]*src[4];
 /* calculate second 8 elements (cofactors) */
 dst[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
 dst[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
 dst[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
 dst[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
 dst[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
 dst[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
 dst[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
 dst[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
 dst[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
 dst[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
 dst[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
 dst[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
 dst[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
 dst[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
 dst[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
 dst[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];
 /* calculate determinant */
 det=src[0]*dst[0]+src[1]*dst[1]+src[2]*dst[2]+src[3]*dst[3];
 /* calculate matrix inverse */
 det = 1/det;
 for (int j = 0; j < 16; j++)
 dst[j] *= det;
}


void matrix4x4_inverse(matrix *a, matrix *result){
    float mat[16], res[16];
    for (int i = 0; i < 16; i++){
        mat[i] = a->array[i/4][i%4];
    }
    Invert2(mat, res);

    matrix_initialize(result, 4, 4);
    for (int i = 0; i < 16; i++){
        result->array[i/4][i%4] = res[i];
    }
}