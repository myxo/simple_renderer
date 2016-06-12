#include "model.h"
#include "tgaimage.h"
#include "geometry.h"
#include "matrix.h"
#include "our_gl.h"

#include <stdio.h>
#include <limits>
#include <sys/time.h>

const int width  = 800;
const int height = 800;
const int depth  = 255;

vec3D light_dir = {1, 0, 1};
vec3D eye       = {1, 1, 2};
vec3D center    = {0, 0, 0};
vec3D up        = {0, 1, 0};

model m;

matrix modelView, Projection, ViewPort, transform;

float varying_intensity[3];
vec2i uv[3];

vec3i vertex_shader(int iface, int nthvert){
    varying_intensity[nthvert] = std::max(0.f, 
                v_scalar_product(m.norms[m.faces[iface][0][nthvert]], light_dir));
    
    vec3D v = m.verts[m.faces[iface][0][nthvert]];
    uv[nthvert] = model_uv(&m, iface, nthvert);

    return get_screen_coords(&transform, v);

}

bool fragment_shader(vec3D bar, TGAColor &color){
    float intensity = varying_intensity[0] * bar.x 
                    + varying_intensity[1] * bar.y 
                    + varying_intensity[2] * bar.z;   // interpolate intensity for the current pixel
    // color = TGAColor(255, 255, 255)*intensity;

    if (intensity < 0) return true;

    vec2i uvP = barycentric_to_world2i(uv, bar);
    color = model_diffuse(&m, uvP) * intensity;
    return false;
}

void transform_matrix_initialization(){
    matrix_initialize(&modelView, 4, 4);
    lookat(eye, center, up, &modelView);

    matrix_initialize(&Projection, 4, 4);
    matrix_identity(&Projection);
    Projection.array[3][2] = -1.0/v_norm(v_sub(eye,center));

    matrix_initialize(&ViewPort, 4, 4);
    viewport(width/8, height/8, width*3/4, height*3/4, &ViewPort);

    get_transform_matrix(&ViewPort, &Projection, &modelView, &transform);
}

int main(int argc, char** argv){
    if (2 == argc) {
        model_load(&m, argv[1]);
    } else {
        // model_load(&m, "obj/test.obj");
        model_load(&m, "obj/african_head.obj");
        // model_load(&m, "obj/Crate1.obj");
    }

    light_dir = v_normilize(light_dir);

    int *zbuffer = new int[width*height];
    for (int i=0; i<width*height; zbuffer[i++] = std::numeric_limits<int>::min());

    TGAImage image(width, height, TGAImage::RGB);

    transform_matrix_initialization();
    
    // time measurements
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);


    for (int i = 0; i < m.fn; i++){
        vec3i screen_coords[3];

        for (int j = 0; j < 3; j++){
            screen_coords[j] = vertex_shader(i, j);
        }
        triangle(&m, screen_coords, uv, image, varying_intensity, zbuffer, fragment_shader);
    }


    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    printf("Rendering time: %ld.%06ld\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);    

    image.flip_vertically(); 
    image.write_tga_file("output.tga");

    delete[] zbuffer;
}