#ifndef _MODEL_H
#define _MODEL_H

#include "geometry.h"
#include "tgaimage.h"

#define N 10000

typedef struct model_{
    vec3D verts[N];
    int faces[N][3][3];
    vec3D norms[N];
    vec3D uv[N];
    TGAImage diffusemap;
    TGAImage normalmap;
    TGAImage specularmap;

    // temporary, until scene struct not created
    vec3D light_dir;

    int vn, fn, nn, un;
} model;

void model_load(model *m, char *filename);
void model_print(model *m);

vec3D model_vertex(model *m, int iface, int nvert);
vec2i model_uv(model *m, int iface, int nvert);
TGAColor model_diffuse(model *m, vec2i uv);
vec3D model_normal_map(model *m, vec2i p);
float model_specular(model *m, vec2i uv);

#endif