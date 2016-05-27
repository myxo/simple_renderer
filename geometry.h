#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

typedef struct vec3D_{
    float x, y, z;
} vec3D;

typedef struct vec2i_{
    int x, y;
} vec2i;

vec3D v_sum(vec3D left, vec3D right);
vec3D v_sub(vec3D left, vec3D right);
vec3D v_vector_product(vec3D a, vec3D b);
float v_scalar_product(vec3D a, vec3D b);
vec3D v_normilize(vec3D v);

#endif
