#include "model.h"
#include "tgaimage.h"
#include "geometry.h"
#include "matrix.h"

#include <stdio.h>
#include <limits>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);

const int width  = 800;
const int height = 800;
const int depth  = 255;

void line(vec2i p0, vec2i p1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(p0.x-p1.x)<std::abs(p0.y-p1.y)) {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    if (p0.x>p1.x) {
        std::swap(p0, p1);
    }

    for (int x=p0.x; x<=p1.x; x++) {
        float t = (x-p0.x)/(float)(p1.x-p0.x);
        int y = p0.y*(1.-t) + p1.y*t;
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
}

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

            TGAColor color_diffuse = model_diffuse(m, uvP) * intensity;
            image.set(P.x, P.y, color_diffuse);
        }
    }
}



int main(int argc, char** argv){
    model m;
    vec3D light_dir = {1, -1, 1};

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

    for (int i = 0; i < m.fn; i++){
        vec3i screen_coords[3];
        vec3D world_coords[3];
        vec3D norm[3];
        for (int j = 0; j < 3; j++){
            vec3D v1 = m.verts[m.faces[i][0][j]];
            screen_coords[j].x = (v1.x + 1.0) * width/2.0;
            screen_coords[j].y = (v1.y + 1.0) * height/2.0;
            screen_coords[j].z = (v1.z + 1.0) * depth/2.0;

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

    float a[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    float b[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    matrix A, B, C;
    matrix_initialize(&A, 3, 3);
    matrix_initialize(&B, 3, 3);
    matrix_set(&A, a, 9);
    matrix_set(&B, b, 9);

    matrix_product(&A, &B, &C);

    matrix_print(&A);
    matrix_print(&B);
    matrix_print(&C);

    matrix_delete(&A);
    matrix_delete(&B);
    matrix_delete(&C);


    // model_print(&m);
    image.flip_vertically(); 
    image.write_tga_file("output.tga");

    delete[] zbuffer;
}