#include "shader.h"
#include "geometry.h"
#include "matrix.h"
#include "model.h"
#include "our_gl.h"

#include <math.h>

extern matrix Transform;


Shader* shader_initialize(model *m, matrix *uniform_Mshadow_, int width, int height, int *depthzbuffer){
    Shader *shader = (Shader *)malloc(sizeof(Shader));

    shader->vertex_shader   = shader_vertex;
    shader->fragment_shader = shader_fragment;
    shader->uniform_Mshadow = uniform_Mshadow_;
    shader->m               = m;
    shader->uniform_width   = width;
    shader->uniform_height  = height;
    shader->uniform_depthzbuffer = depthzbuffer;

    return shader;
}




vec3i shader_vertex(Shader *shader, int iface, int nthvert){
    shader->varying_uv[nthvert] = model_uv(shader->m, iface, nthvert);
    vec3D v = model_vertex(shader->m, iface, nthvert);
    shader->varying_tri[nthvert] = get_screen_coords(&Transform, v);
    return shader->varying_tri[nthvert];
}



bool shader_fragment(Shader *shader, vec3D bar, TGAColor &color){
    vec3i w = barycentric_to_world3i(shader->varying_tri, bar);
    vec3D q = {w.x, w.y, w.z};
    vec3D sb_p = point_Transform(shader->uniform_Mshadow, q);
    int idx = int(sb_p.x) + int(sb_p.y)*shader->uniform_width; // index in the shadowbuffer array
    float shadow = 0.1 + 0.9*(shader->uniform_depthzbuffer[idx] < sb_p.z + 43.72); 

    vec2i uvP = barycentric_to_world2i(shader->varying_uv, bar);
    vec3D norm = model_normal_map(shader->m, uvP);

    vec3D r = v_sub(v_scalar_multiply(norm, (v_scalar_product(norm, shader->m->light_dir) * 2.0)), shader->m->light_dir);   // reflected light
    r = v_normilize(r);


    float spec = pow(std::max(r.z, 0.0f), model_specular(shader->m, uvP));
    // float spec = 0;
    float diff = std::max(0.f, v_scalar_product(norm, shader->m->light_dir));


    color = model_diffuse(shader->m, uvP);
    // color = TGAColor(255, 255, 255);
    for (int i=0; i<3; i++) 
        color[i] = std::min<float>(1 + color[i]*shadow*(0.9*diff + 0.6*spec), 255);
        // color[i] = std::min<float>(1 + color[i]*(1.0*diff + 0.9*spec), 255);

    return false;
}




Shader* depth_shader_initialize(model *m, int depth){
    Shader *shader = (Shader *)malloc(sizeof(Shader));

    shader->vertex_shader   = depth_shader_vertex;
    shader->fragment_shader = depth_shader_fragment;
    shader->m = m;
    shader->uniform_depth = depth;
    return shader;
}


vec3i depth_shader_vertex(Shader *shader, int iface, int nthvert) {
    vec3D v = model_vertex(shader->m, iface, nthvert);
    vec3i vs = get_screen_coords(&Transform, v);
    shader->varying_tri[nthvert] = vs;
    return vs;
}

bool depth_shader_fragment(Shader *shader, vec3D bar, TGAColor &color) {
    float z =   shader->varying_tri[0].z * bar.x + 
                shader->varying_tri[1].z * bar.y + 
                shader->varying_tri[2].z * bar.z;
    color = TGAColor(255, 255, 255)*(z/shader->uniform_depth);
    return false;
}


