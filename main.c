#include "model.h"
#include "tgaimage.h"
#include "geometry.h"
#include "matrix.h"
#include "our_gl.h"

#include <stdio.h>
#include <limits>
#include <sys/time.h>
#include <math.h>

const int width  = 800;
const int height = 800;
const int depth  = 2000;

vec3D light_dir = {-2, 5, 4};
vec3D eye       = {2, 1, 3};
vec3D center    = {0, 0, 0};
vec3D up        = {0, 1, 0};

model m;

matrix modelView, Projection, ViewPort, transform, M, MIT;

int *depthzbuffer = new int[width*height];

//-------------------
// shader
//-------------------

float varying_intensity[3];
vec2i varying_uv[3];
vec3i varying_tri2[3];

matrix uniform_Mshadow;

vec3i shader_vertex(int iface, int nthvert){
    varying_intensity[nthvert] = std::max(0.f, 
                v_scalar_product(m.norms[m.faces[iface][0][nthvert]], light_dir));
    varying_uv[nthvert] = model_uv(&m, iface, nthvert);

    vec3D v = m.verts[m.faces[iface][0][nthvert]];
    varying_tri2[nthvert] = get_screen_coords(&transform, v);
    return get_screen_coords(&transform, v);

}

bool shader_fragment(vec3D bar, TGAColor &color){

    // Vec4f sb_p = uniform_Mshadow*embed<4>(varying_tri*bar); // corresponding point in the shadow buffer
    // sb_p = sb_p/sb_p[3];
    // vec3D sb_p = point_transform(&uniform_Mshadow, barycentric_to_world3D(varying_tri2, bar));
    vec3i w = barycentric_to_world3i(varying_tri2, bar);
    vec3D q = {w.x, w.y, w.z};
    // printf("%f %f %f\n", varying_tri2.x, varying_tri2.y, varying_tri2.z);
    // printf("%f %f %f\n", bar.x, bar.y, bar.z);
    // printf("%f %f %f\n", q.x, q.y, q.z);
    vec3D sb_p = point_transform(&uniform_Mshadow, q);
    int idx = int(sb_p.x) + int(sb_p.y)*width; // index in the shadowbuffer array
    float shadow = 0.1 + 0.9*(depthzbuffer[idx] < sb_p.z + 43.72); 
    // printf("%f\n", shadow);

    vec2i uvP = barycentric_to_world2i(varying_uv, bar);
    vec3D norm = model_normal_map(&m, uvP);
    // norm = v_normilize(norm);

    vec3D r = v_sub(v_scalar_multiply(norm, (v_scalar_product(norm, light_dir) * 2.0)), light_dir);   // reflected light
    r = v_normilize(r);


    float spec = pow(std::max(r.z, 0.0f), model_specular(&m, uvP));
    // float spec = 0;
    float diff = std::max(0.f, v_scalar_product(norm, light_dir));

    // if (intensity < 0) intensity = 0;

    color = model_diffuse(&m, uvP);
    // color = TGAColor(255, 255, 255);
    for (int i=0; i<3; i++) 
        color[i] = std::min<float>(1 + color[i]*shadow*(0.9*diff + 0.6*spec), 255);
        // printf("%f\n", shadow);
        // color[i] = std::min<float>(1 + color[i]*(1.0*diff + 0.9*spec), 255);

    return false;
}

//-------------------------
// depth shader
//-------------------------
vec3i varying_tri[3];

vec3i depth_shader_vertex(int iface, int nthvert) {
    vec3D v = m.verts[m.faces[iface][0][nthvert]];
    vec3i vs = get_screen_coords(&transform, v);
    varying_tri[nthvert] = vs;
    // printf("%d\n", vs.z);
    return vs;
}

bool depth_shader_fragment(vec3D bar, TGAColor &color) {
    float z = varying_tri[0].z * bar.x + varying_tri[1].z * bar.y + varying_tri[2].z * bar.z;
    color = TGAColor(255, 255, 255)*(z/depth);
    return false;
}




void transform_matrix_initialization(){
    matrix_initialize(&modelView, 4, 4);
    lookat(eye, center, up, &modelView);

    matrix_initialize(&Projection, 4, 4);
    matrix_identity(&Projection);
    // Projection.array[3][2] = -1.0/v_norm(v_sub(eye,center));

    matrix_initialize(&ViewPort, 4, 4);
    viewport(width/8, height/8, width*3/4, height*3/4, &ViewPort);

    // get_transform_matrix(&ViewPort, &Projection, &modelView, &transform);

    // matrix_print(&transform);

    matrix *list[3] = {&ViewPort, &Projection, &modelView};
    matrix_list_product(list, 3, &transform);
    // matrix_print(tmp);

    // matrix_initialize(&M, 4, 4);
    // matrix_product(&Projection, &modelView, &M);
}



void depth_transform_matrix_initialization(){
    matrix_initialize(&modelView, 4, 4);
    lookat(light_dir, center, up, &modelView);

    matrix_initialize(&Projection, 4, 4);
    matrix_identity(&Projection);
    // Projection.array[3][2] = -1.0/v_norm(v_sub(eye,center));

    matrix_initialize(&ViewPort, 4, 4);
    viewport(width/8, height/8, width*3/4, height*3/4, &ViewPort);

    matrix *list[3] = {&ViewPort, &Projection, &modelView};
    matrix_list_product(list, 3, &transform);

    // matrix_initialize(&M, 4, 4);
    // matrix_product(&Projection, &modelView, &M);
}

int main(int argc, char** argv){
    if (2 == argc) {
        model_load(&m, argv[1]);
    } else {
        model_load(&m, "obj/african_head/african_head.obj");
        // model_load(&m, "obj/alduin/alduin.obj");
    }

    light_dir = v_normilize(light_dir);

    printf("hello\n");

    int *zbuffer = new int[width*height];
    for (int i=0; i<width*height; zbuffer[i++] = std::numeric_limits<int>::min());

    
    for (int i=0; i<width*height; depthzbuffer[i++] = std::numeric_limits<int>::min());

    TGAImage image(width, height, TGAImage::RGB);
    TGAImage depthimage(width, height, TGAImage::RGB);

    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);
    { // depth shader
        depth_transform_matrix_initialization();

        for (int i = 0; i < m.fn; i++){
            vec3i screen_coords[3];

            for (int j = 0; j < 3; j++){
                screen_coords[j] = depth_shader_vertex(i, j);
            }
            triangle(&m, screen_coords, varying_uv, depthimage, varying_intensity, depthzbuffer, depth_shader_fragment);
        }
    }
    depthimage.flip_vertically(); 
    depthimage.write_tga_file("depthimage.tga");

    matrix *list[3] = {&ViewPort, &Projection, &modelView};

    // matrix_print(&ViewPort);
    // matrix_print(&Projection);
    // matrix_print(&modelView);
    
    matrix_list_product(list, 3, &M);

    // for (int i=0; i<width*height; i++) printf("%d ", depthzbuffer[i]);

    {
        transform_matrix_initialization();
        matrix *list2[3] = {&ViewPort, &Projection, &modelView};
        matrix_list_product(list2, 3, &MIT);

        

        matrix tmp;
        matrix4x4_inverse(&MIT, &tmp);
        matrix_product(&M, &tmp, &uniform_Mshadow);
        matrix_print(&uniform_Mshadow);

        for (int i = 0; i < m.fn; i++){
            vec3i screen_coords[3];

            for (int j = 0; j < 3; j++){
                screen_coords[j] = shader_vertex(i, j);
            }
            triangle(&m, screen_coords, varying_uv, image, varying_intensity, zbuffer, shader_fragment);
        }

    }

    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    printf("Rendering time: %ld.%06ld\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);    

    image.flip_vertically(); 
    image.write_tga_file("output.tga");

    delete[] zbuffer;
}