#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

typedef struct vec3D_{
    float x, y, z;
} vec3D;

typedef struct vec2i_{
    int x, y;
} vec2i;

typedef struct vec3i_{
    int x, y, z;
} vec3i;

vec3D v_sum(vec3D left, vec3D right);
vec3D v_sub(vec3D left, vec3D right);
vec3D v_scalar_multiply(vec3D a, float alpha);
vec3D v_vector_product(vec3D a, vec3D b);
float v_scalar_product(vec3D a, vec3D b);
// TODO typo, fix it
vec3D v_normilize(vec3D v);
float v_norm(vec3D a);


vec3i vi_sum(vec3i left, vec3i right);
vec3i vi_sub(vec3i left, vec3i right);
vec3i vi_scalar_multiply(vec3i a, float alpha);
#endif
