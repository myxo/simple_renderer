#include "geometry.h"

#include <math.h>


vec3D v_sum(vec3D left, vec3D right){
    vec3D result = {left.x + right.x, left.y + right.y, left.z + right.z};
    return result;
}

vec3D v_sub(vec3D left, vec3D right){
    vec3D result = {left.x - right.x, left.y - right.y, left.z - right.z};
    return result;
}

vec3D v_scalar_multiply(vec3D a, float alpha){
    vec3D result = {a.x * alpha, a.y * alpha, a.z * alpha};
    return result;
}

vec3D v_vector_product(vec3D a, vec3D b){
    vec3D result = {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
    return result;
}

float v_scalar_product(vec3D a, vec3D b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

vec3D v_normilize(vec3D v){
    float s = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    vec3D result = {v.x/s, v.y/s, v.z/s};
    return result;
}


vec3i vi_sum(vec3i left, vec3i right){
    vec3i result = {left.x + right.x, left.y + right.y, left.z + right.z};
    return result;
}

vec3i vi_sub(vec3i left, vec3i right){
    vec3i result = {left.x - right.x, left.y - right.y, left.z - right.z};
    return result;
}

vec3i vi_scalar_multiply(vec3i a, float alpha){
    vec3i result = {(int)(a.x * alpha), (int)(a.y * alpha), (int)(a.z * alpha)};
    return result;
}