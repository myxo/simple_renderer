#ifndef _MODEL_H
#define _MODEL_H

#include "geometry.h"

#define N 10000

typedef struct model_{
    vec3D verts[N];
    int faces[N][3];
    int vn, fn;
} model;

void model_load(model *m, char *filename);
void model_print(model *m);

#endif