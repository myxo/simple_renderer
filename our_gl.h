#pragma once

#include "geometry.h"
#include "matrix.h"
#include "model.h"

void viewport(int x, int y, int w, int h, matrix *res);
void lookat(vec3D eye, vec3D center, vec3D up, matrix *result);

void triangle(model *m, vec3i pts[3], vec2i uv_pts[3], 
        TGAImage &image, float *intensity_array, int *zbuffer, 
        bool (*fragment_shader)(vec3D bar, TGAColor &color));

void get_transform_matrix(matrix *ViewPort, matrix *Projection, matrix *ModelView, matrix *result);
vec3i get_screen_coords(matrix *transform, vec3D v);

vec2i barycentric_to_world2i(vec2i *pts, vec3D bar);