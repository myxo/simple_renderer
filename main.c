#include "model.h"
#include "tgaimage.h"
#include "geometry.h"

#include <stdio.h>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);

const int width  = 800;
const int height = 800;

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


void triangle(vec2i t0, vec2i t1, vec2i t2, TGAImage &image, TGAColor color) {
    if (t0.y==t1.y && t0.y==t2.y) return; 
    // sort the vertices, t0, t1, t2 lower-to-upper (bubblesort yay!)
    if (t0.y>t1.y) std::swap(t0, t1);
    if (t0.y>t2.y) std::swap(t0, t2);
    if (t1.y>t2.y) std::swap(t1, t2);
    int total_height = t2.y-t0.y;
    for (int i=0; i<total_height; i++) {
        bool second_half = i>t1.y-t0.y || t1.y==t0.y;
        int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
        float alpha = (float)i/total_height;
        float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height;
        int x1 =               t0.x + (t2.x-t0.x)*alpha;
        int x2 = second_half ? t1.x + (t2.x-t1.x)*beta : t0.x + (t1.x-t0.x)*beta;
        if (x1 > x2) std::swap(x1, x2);
        for (int x=x1; x<=x2; x++) {
            image.set(x, t0.y+i, color); // attention, due to int casts t0.y+i != A.y
        }
    }
}




int main(int argc, char** argv){
    model m;
    vec3D light_dir = {0, 0, -1};

    if (2 == argc) {
        model_load(&m, argv[1]);
    } else {
        model_load(&m, "obj/african_head.obj");
    }


    TGAImage image(width, height, TGAImage::RGB);

    for (int i = 0; i < m.fn; i++){
        vec2i screen_coords[3];
        vec3D world_coords[3];
        for (int j = 0; j < 3; j++){
            vec3D v1 = m.verts[m.faces[i][j]];
            screen_coords[j].x = (v1.x + 1.0) * width/2.0;
            screen_coords[j].y = (v1.y + 1.0) * height/2.0;

            world_coords[j] = m.verts[m.faces[i][j]];
        }

        vec3D n = v_vector_product(v_sub(world_coords[2], world_coords[0]), 
                v_sub(world_coords[1], world_coords[0]));

        n = v_normilize(n);
        float intensity = v_scalar_product(n, light_dir);
        if (intensity>0) {
            triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity*255, intensity*255, intensity*255, 255));
        }
    }


    // model_print(&m);
    image.flip_vertically(); 
    image.write_tga_file("output.tga");
}