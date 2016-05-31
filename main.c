#include "model.h"
#include "tgaimage.h"
#include "geometry.h"

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
        TGAImage &image, TGAColor color, float intensity, int *zbuffer){
    if (t0.y==t1.y && t0.y==t2.y) return; 
    // sort the vertices, t0, t1, t2 lower-to-upper (bubblesort yay!)

    vec3i bboxmin, bboxmax;
    vec3i pts[3] = {t0, t1, t2};
    find_triangle_bounding_box(pts, &bboxmin, &bboxmax, image.get_width(), image.get_height());

    vec2i P;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
            vec3D bc_screen  = barycentric(pts, P);

            float frag_depth = bc_screen.x*pts[0].z + bc_screen.y*pts[1].z + bc_screen.z*pts[2].z;

            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 || zbuffer[P.x+P.y*image.get_width()]>frag_depth) continue;
            zbuffer[P.x+P.y*image.get_width()] = frag_depth;
            image.set(P.x, P.y, color);
        }
    }
}


// // лучше разобраться, что здесь происходит
// void triangle_old(model *m, vec3i t0, vec3i t1, vec3i t2, vec2i uv0, vec2i uv1, vec2i uv2, 
//         TGAImage &image, TGAColor color, float intensity, int *zbuffer) {

//     if (t0.y==t1.y && t0.y==t2.y) return; 
//     // sort the vertices, t0, t1, t2 lower-to-upper (bubblesort yay!)
//     if (t0.y>t1.y) { std::swap(t0, t1); std::swap(uv0, uv1); }
//     if (t0.y>t2.y) { std::swap(t0, t2); std::swap(uv0, uv2); }
//     if (t1.y>t2.y) { std::swap(t1, t2); std::swap(uv1, uv2); }
//     int total_height = t2.y-t0.y;

//     for (int i=0; i<total_height; i++) {
//         bool second_half = i>t1.y-t0.y || t1.y==t0.y;
//         int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
//         float alpha = (float)i/total_height;
//         float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height;
        
//         vec3i A =     vi_sum(t0, vi_scalar_multiply(vi_sub(t2,t0), alpha));
//         vec3i B = second_half 
//                     ? vi_sum(t1, vi_scalar_multiply(vi_sub(t2,t1), beta))
//                     : vi_sum(t0, vi_scalar_multiply(vi_sub(t1,t0), beta));
        
//         vec2i uvA = {uv0.x + (int)(uv2.x-uv0.x)*alpha, uv0.y + (int)(uv2.y-uv0.y)*alpha};
//         vec2i uvB;
//         // uvB.x = second_half ? uv1.x + (uv2.x-uv1.x)*beta : uv0.x + (uv1.x-uv0.x)*beta;
//         // uvB.x = second_half ? uv1.y + (uv2.y-uv1.y)*beta : uv0.y + (uv1.y-uv0.y)*beta;
        
//         if (A.x > B.x) { std::swap(A, B); std::swap(uvA, uvB); }

//         for (int j=A.x; j<=B.x; j++) {
//             float phi = B.x==A.x ? 1. : (float)(j-A.x)/(float)(B.x-A.x);
//             // vec3i P = vi_sum(A, vi_scalar_multiply(vi_sub(B,A), phi));
//             // check this place. Differents with source

//             int z = (int)((float)A.z + float(B.z - A.z)*phi + 0.5);

//             // vec2i uvP = { uvA.x + (uvB.x-uvA.x)*phi, uvA.y + (uvB.y-uvA.y)*phi};

//             int idx = j + (t0.y + i)*width;
//             if (zbuffer[idx] < z) {
//                 zbuffer[idx] = z;
//                 // image.set(P.x, P.y, color);
//                 TGAColor c = model_diffuse(m, uvP);
//                 c.r *= intensity;
//                 c.g *= intensity; 
//                 c.b *= intensity;
//                 // image.set(j, t0.y + i, c);
//                 image.set(j, t0.y + i, color);
//             }
//         }
//     }
// }




int main(int argc, char** argv){
    model m;
    vec3D light_dir = {0, 0, -1};

    if (2 == argc) {
        model_load(&m, argv[1]);
    } else {
        // model_load(&m, "obj/test.obj");
        model_load(&m, "obj/african_head.obj");
    }

    int *zbuffer = new int[width*height];
    for (int i=0; i<width*height; i++) {
        zbuffer[i] = std::numeric_limits<int>::min();
    }


    TGAImage image(width, height, TGAImage::RGB);

    for (int i = 0; i < m.fn; i++){
        vec3i screen_coords[3];
        vec3D world_coords[3];
        for (int j = 0; j < 3; j++){
            vec3D v1 = m.verts[m.faces[i][0][j]];
            screen_coords[j].x = (v1.x + 1.0) * width/2.0;
            screen_coords[j].y = (v1.y + 1.0) * height/2.0;
            screen_coords[j].z = (v1.z + 1.0) * depth/2.0;

            world_coords[j] = v1;
        }

        vec3D n = v_vector_product(v_sub(world_coords[2], world_coords[0]), 
                v_sub(world_coords[1], world_coords[0]));

        n = v_normilize(n);
        float intensity = v_scalar_product(n, light_dir);
        if (intensity>0) {
            vec2i uv[3];
            for (int k = 0; k < 3; k++){
                uv[k] = model_uv(&m, i, k);
            }
            triangle(&m, screen_coords[0], screen_coords[1], screen_coords[2], uv[0], uv[1], uv[2], image, TGAColor(intensity*255, intensity*255, intensity*255, 255), intensity, zbuffer);
        }
    }


    // model_print(&m);
    image.flip_vertically(); 
    image.write_tga_file("output.tga");

    delete[] zbuffer;
}