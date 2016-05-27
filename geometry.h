#ifndef _GEOMETRY_H
#define _GEOMETRY_H

typedef struct vec3D_{
    float x, y, z;
} vec3D;

typedef struct vec2i_{
    int x, y;
} vec2i;

void sum(vec3D *left, vec3D *right);
// void sub(vec3D *left, vec3D *right);
// void scalar(vec3D *left, vec3D *right);

#endif
