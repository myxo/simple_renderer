#include "model.h"
#include "tgaimage.h"
#include "geometry.h"
#include "matrix.h"
#include "our_gl.h"
#include "shader.h"

#include <stdio.h>
#include <limits>
#include <sys/time.h>
#include <math.h>

const int width  = 800;
const int height = 800;

vec3D light_dir = {-2, 5, 4};
vec3D eye       = {2, 1, 3};
vec3D center    = {0, 0, 0};
vec3D up        = {0, 1, 0};


int main(int argc, char** argv){
    model m;
    if (2 == argc) {
        model_load(&m, argv[1]);
    } else {
        model_load(&m, "obj/african_head/african_head.obj");
        // model_load(&m, "obj/alduin/alduin.obj");
    }

    printf("%d vertex, %d faces\n\n", m.vn, m.fn);
    light_dir = v_normilize(light_dir);
    m.light_dir = light_dir;


    int *zbuffer        = new int[width*height];
    int *depthzbuffer   = new int[width*height];
    for (int i=0; i<width*height; zbuffer[i++]      = std::numeric_limits<int>::min());
    for (int i=0; i<width*height; depthzbuffer[i++] = std::numeric_limits<int>::min());

    TGAImage image      (width, height, TGAImage::RGB);
    TGAImage depthimage (width, height, TGAImage::RGB);

    matrix_initialization();

    // time measurement
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);

    // depth shader
    { 
        lookat(light_dir, center, up);
        projection(0);
        viewport(width/8, height/8, width*3/4, height*3/4);
        transform();

        Shader *depth_shader = depth_shader_initialize(&m, depth);

        for (int i = 0; i < m.fn; i++){
            vec3i screen_coords[3];

            for (int j = 0; j < 3; j++){
                screen_coords[j] = depth_shader->vertex_shader(depth_shader, i, j);
            }
            triangle(&m, depth_shader, screen_coords, depthimage, depthzbuffer);
        }
    }


    matrix *list[3] = {&ViewPort, &Projection, &modelView};
    matrix_list_product(list, 3, &M);

    // main shader
    { 
        lookat(eye, center, up);
        // projection(-1.0/v_norm(v_sub(eye,center)));
        projection(0);
        viewport(width/8, height/8, width*3/4, height*3/4);
        transform();


        matrix *list2[3] = {&ViewPort, &Projection, &modelView};
        matrix_list_product(list2, 3, &MIT);

        matrix tmp, uniform_Mshadow;
        matrix4x4_inverse(&MIT, &tmp);
        matrix_product(&M, &tmp, &uniform_Mshadow);
        // matrix_print(&uniform_Mshadow);

        Shader *shader = shader_initialize(&m, &uniform_Mshadow, width, height, depthzbuffer);

        for (int i = 0; i < m.fn; i++){
            vec3i screen_coords[3];

            for (int j = 0; j < 3; j++){
                screen_coords[j] = shader->vertex_shader(shader, i, j);
            }
            triangle(&m, shader, screen_coords, image, zbuffer);
        }

    }

    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    printf("Rendering time: %ld.%06ld\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);    


    depthimage.flip_vertically(); 
    depthimage.write_tga_file("depthimage.tga");
    image.flip_vertically(); 
    image.write_tga_file("output.tga");

    delete[] zbuffer;
    delete[] depthzbuffer;
}