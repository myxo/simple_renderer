#include "model.h"
#include "tgaimage.h"

#include <stdio.h>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);

const int width  = 200;
const int height = 200;

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

    if (2 == argc) {
        model_load(&m, argv[1]);
    } else {
        model_load(&m, "obj/african_head.obj");
    }


    TGAImage image(width, height, TGAImage::RGB);

    // for (int i = 0; i < m.fn; i++){
    //     for (int j = 0; j < 3; j++){
    //         vec3D v1 = m.verts[m.faces[i][j]];
    //         vec3D v2 = m.verts[m.faces[i][(j+1)%3]];

    //         int x0 = (v1.x + 1.0) * width/2.;
    //         int y0 = (v1.y + 1.0) * height/2.;
    //         int x1 = (v2.x + 1.0) * width/2.;
    //         int y1 = (v2.y + 1.0) * height/2.;
    //         line(x0, y0, x1, y1, image, white);
    //     }
    // }

    vec2i v1 = {10, 70};
    vec2i v2 = {50, 160};
    vec2i v3 = {70, 80};
    vec2i v4 = {180, 50};
    vec2i v5 = {150, 1};
    vec2i v6 = {70, 180};

    vec2i t0[3] = {v1, v2, v3};
    vec2i t1[3] = {v4, v5, v6};

    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);


    // model_print(&m);
    image.flip_vertically(); 
    image.write_tga_file("output.tga");
}