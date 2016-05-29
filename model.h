#ifndef _MODEL_H
#define _MODEL_H

#include "geometry.h"
#include "tgaimage.h"

#define N 50000

typedef struct model_{
    vec3D verts[N];
    int faces[N][3][3];
    vec3D norms[N];
    vec3D uv[N];
    TGAImage diffusemap;

    int vn, fn, nn, un;
} model;

void model_load(model *m, char *filename);
void model_print(model *m);

vec2i model_uv(model *m, int iface, int nvert);
TGAColor model_diffuse(model *m, vec2i uv);

#endif