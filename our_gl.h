#pragma once

#include "geometry.h"
#include "matrix.h"
#include "model.h"
#include "shader.h"


extern matrix modelView, Projection, ViewPort, Transform, M, MIT;
extern int depth; // zbuffer depth

void viewport(int x, int y, int w, int h);
void lookat(vec3D eye, vec3D center, vec3D up);
void projection(float alpha);
void transform();
void matrix_initialization();

void triangle(model *m, Shader *shader, vec3i pts[3], TGAImage &image, int *zbuffer);

// void get_Transform_matrix(matrix *ViewPort, matrix *Projection, matrix *ModelView, matrix *result);
vec3i get_screen_coords(matrix *Transform, vec3D v);
vec3D point_Transform(matrix *mat, vec3D p);

vec2i barycentric_to_world2i(vec2i *pts, vec3D bar);
vec3i barycentric_to_world3i(vec3i *pts, vec3D bar);
vec3D barycentric_to_world3D(vec3D *pts, vec3D bar);
vec3D barycentric(vec3i *pts, vec2i P);