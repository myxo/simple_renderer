#pragma once

#include "geometry.h"
#include "matrix.h"
#include "model.h"

typedef struct shader{
    vec2i varying_uv[3];
    vec3i varying_tri[3];

    int uniform_width, uniform_height, uniform_depth;
    int *uniform_depthzbuffer;
    matrix *uniform_Mshadow;
    model *m;

    vec3i (*vertex_shader)(shader *shader, int iface, int nthvert);
    bool (*fragment_shader)(shader *shader, vec3D bar, TGAColor &color);
} Shader;



Shader* shader_initialize   (model *m, matrix *uniform_Mshadow_, int width, int height, int *depthzbuffer);
vec3i   shader_vertex       (Shader *shader, int iface, int nthvert);
bool    shader_fragment     (Shader *shader, vec3D bar, TGAColor &color);

Shader* depth_shader_initialize (model *m, int depth);
vec3i   depth_shader_vertex     (Shader *shader, int iface, int nthvert);
bool    depth_shader_fragment   (Shader *shader, vec3D bar, TGAColor &color);
