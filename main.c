#include "model.h"
#include "tgaimage.h"
#include "geometry.h"
#include "matrix.h"

#include <stdio.h>
#include <limits>

const int width  = 800;
const int height = 800;
const int depth  = 255;

void find_triangle_bounding_box(vec3i *pts, vec3i *bboxmin, vec3i *bboxmax, int width, int height){
    bboxmin->x = width - 1;
    bboxmin->y = height - 1;
    bboxmax->x = 0;
    bboxmax->y = 0;
    // vec3i clamp     = {image.get_width()-1, image.get_height()-1};
    for (int i = 0; i < 3; i++){
        bboxmin->x = std::max(0, std::min(bboxmin->x, pts[i].x));
        bboxmin->y = std::max(0, std::min(bboxmin->y, pts[i].y));

        bboxmax->x = std::min(width-1,  std::max(bboxmax->x, pts[i].x));
        bboxmax->y = std::min(height-1, std::max(bboxmax->y, pts[i].y));
        
    }
}


void viewport(int x, int y, int w, int h, matrix *res) {
    matrix_identity(res);
    res->array[0][3] = x+w/2.f;
    res->array[1][3] = y+h/2.f;
    res->array[2][3] = depth/2.f;

    res->array[0][0] = w/2.f;
    res->array[1][1] = h/2.f;
    res->array[2][2] = depth/2.f;
}


void lookat(vec3D eye, vec3D center, vec3D up, matrix *result) {
    vec3D z = v_sub(eye,center);
    z = v_normilize(z);
    vec3D x = v_vector_product(up,z);
    x = v_normilize(x);
    vec3D y = v_vector_product(z,x);
    y = v_normilize(y);

    matrix Minv, Tr; 
    matrix_initialize(&Minv, 4, 4);
    matrix_initialize(&Tr, 4, 4);
    matrix_identity(&Minv);
    matrix_identity(&Tr);

    float array[16] = {x.x, x.y, x.z, 0, y.x, y.y, y.z, 0, z.x, z.y, z.z, 0, 0, 0, 0, 1};
    matrix_set(&Minv, array, 16);
        
    Tr.array[0][3] = -center.x;
    Tr.array[1][3] = -center.y;
    Tr.array[2][3] = -center.z;
    matrix_product(&Minv, &Tr, result);

}


vec2i barycentric_to_world2i(vec2i *pts, vec3D bar){
    vec2i result;
    result.x = pts[0].x * bar.x + pts[1].x * bar.y + pts[2].x * bar.z;
    result.y = pts[0].y * bar.x + pts[1].y * bar.y + pts[2].y * bar.z;
    return result;
}

float barycentric_to_float(float *intensity, vec3D bar){
    return intensity[0] * bar.x + intensity[1] * bar.y + intensity[2] * bar.z;
}

vec3D barycentric(vec3i *pts, vec2i P) {
    vec3D a = {pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x};
    vec3D b = {pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y};

    vec3D u = v_vector_product(a, b);

    if (u.z < 1 && u.z > -1) {
        vec3D res = {-1, 1, 1};
        return res; // triangle is degenerate, in this case return smth with negative coordinates
    }

    vec3D res = {1.0 - (u.x+u.y)/u.z, u.y/u.z, u.x/u.z};
    return res;
}

void triangle(model *m, vec3i t0, vec3i t1, vec3i t2, vec2i uv0, vec2i uv1, vec2i uv2, 
        TGAImage &image, float *intensity_array, int *zbuffer){

    vec3i bboxmin, bboxmax;
    vec3i pts[3] = {t0, t1, t2};
    find_triangle_bounding_box(pts, &bboxmin, &bboxmax, image.get_width(), image.get_height());

    vec2i P;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
            vec3D bc_screen  = barycentric(pts, P);

            vec2i uv_pts[3] = {uv0, uv1, uv2};
            vec2i uvP = barycentric_to_world2i(uv_pts, bc_screen);
            float frag_depth = bc_screen.x*pts[0].z + bc_screen.y*pts[1].z + bc_screen.z*pts[2].z;

            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 
                    || zbuffer[P.x + P.y*image.get_width()] > frag_depth) continue;

            zbuffer[P.x+P.y*image.get_width()] = frag_depth;

            float intensity = barycentric_to_float(intensity_array, bc_screen);
            if (intensity < 0) continue;

            TGAColor color_diffuse = model_diffuse(m, uvP);
            image.set(P.x, P.y, color_diffuse * intensity);
        }
    }
}

void get_transform_matrix(matrix *ViewPort, matrix *Projection, matrix *ModelView, matrix *result){
    matrix pre_result;
    matrix_product(ViewPort, Projection, &pre_result);
    matrix_product(&pre_result, ModelView, result);

    matrix_delete(&pre_result);
}

vec3i get_screen_coords(matrix *transform, vec3D v){
    matrix V;
    matrix_initialize(&V, 4, 1);
    float array[4] = {v.x, v.y, v.z, 1};
    matrix_set(&V, array, 4);

    matrix result;
    
    matrix_product(transform, &V, &result);

    vec3i res = {result.array[0][0] / result.array[3][0], 
                result.array[1][0] / result.array[3][0], 
                result.array[2][0] / result.array[3][0]};
    return res;
}

int main(int argc, char** argv){
    model m;
    vec3D light_dir = {-1,1,1};
    vec3D eye       = {1,1,1};
    vec3D center    = {0,0,0};


    if (2 == argc) {
        model_load(&m, argv[1]);
    } else {
        // model_load(&m, "obj/test.obj");
        model_load(&m, "obj/african_head.obj");
        // model_load(&m, "obj/Crate1.obj");
    }

    int *zbuffer = new int[width*height];
    for (int i=0; i<width*height; zbuffer[i++] = std::numeric_limits<int>::min());


    TGAImage image(width, height, TGAImage::RGB);

    matrix modelView;
    matrix_initialize(&modelView, 4, 4);
    vec3D tmp = {0, 1, 0};
    lookat(eye, center, tmp, &modelView);
    // matrix_print(&modelView);

    matrix Projection;
    matrix_initialize(&Projection, 4, 4);
    matrix_identity(&Projection);

    Projection.array[3][2] = -1.0/v_norm(v_sub(eye,center));
    // matrix_print(&Projection);

    matrix ViewPort;
    matrix_initialize(&ViewPort, 4, 4);
    viewport(width/8, height/8, width*3/4, height*3/4, &ViewPort);
    // matrix_print(&ViewPort);

    matrix transform;
    get_transform_matrix(&ViewPort, &Projection, &modelView, &transform);

    matrix_print(&transform);

    for (int i = 0; i < m.fn; i++){
        vec3i screen_coords[3];
        vec3D world_coords[3];
        vec3D norm[3];
        for (int j = 0; j < 3; j++){
            vec3D v1 = m.verts[m.faces[i][0][j]];
            // screen_coords[j].x = (v1.x + 1.0) * width/2.0;
            // screen_coords[j].y = (v1.y + 1.0) * height/2.0;
            // screen_coords[j].z = (v1.z + 1.0) * depth/2.0;

            screen_coords[j] = get_screen_coords(&transform, v1);
            // printf("%d %d %d | %f %f %f\n", screen_coords[j].x, screen_coords[j].y, screen_coords[j].z, v1.x, v1.y, v1.z);
            world_coords[j] = v1;

            norm[j] = m.norms[m.faces[i][0][j]];
        }

        float intensity[3] = {0};
        for (int j = 0; j < 3; j++){
            intensity[j] = v_scalar_product(norm[j], light_dir);
        }
        if (intensity>0) {
            vec2i uv[3];
            for (int k = 0; k < 3; k++){
                uv[k] = model_uv(&m, i, k);
            }
            triangle(&m, screen_coords[0], screen_coords[1], screen_coords[2], uv[0], uv[1], uv[2], image, intensity, zbuffer);
        }
    }




    // model_print(&m);
    image.flip_vertically(); 
    image.write_tga_file("output.tga");

    delete[] zbuffer;
}