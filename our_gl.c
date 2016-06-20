#include "our_gl.h"

#include "geometry.h"
#include "matrix.h"
#include "model.h"
#include "shader.h"


matrix modelView, Projection, ViewPort, Transform, M, MIT;
int depth = 2000;


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



void viewport(int x, int y, int w, int h) {
    matrix_identity(&ViewPort);
    ViewPort.array[0][3] = x+w/2.f;
    ViewPort.array[1][3] = y+h/2.f;
    ViewPort.array[2][3] = depth/2.f;

    ViewPort.array[0][0] = w/2.f;
    ViewPort.array[1][1] = h/2.f;
    ViewPort.array[2][2] = depth/2.f;
}


// Make ModelView matrix
void lookat(vec3D eye, vec3D center, vec3D up) {
    vec3D z = v_normilize(v_sub(eye,center));
    vec3D x = v_normilize(v_vector_product(up,z));
    vec3D y = v_normilize(v_vector_product(z,x));

    float array[16] = { x.x,    x.y,    x.z, -center.x, 
                        y.x,    y.y,    y.z, -center.y, 
                        z.x,    z.y,    z.z, -center.z, 
                        0,      0,      0,    1};
    matrix_set(&modelView, array, 16);
}

void projection(float alpha){
    matrix_identity(&Projection);
    Projection.array[3][2] = alpha;
}

void transform(){
    matrix *list[3] = {&ViewPort, &Projection, &modelView};
    matrix_list_product(list, 3, &Transform);
}


void matrix_initialization(){
    matrix_initialize(&modelView, 4, 4);
    matrix_initialize(&Projection, 4, 4);
    matrix_initialize(&ViewPort, 4, 4);
}


vec3D point_Transform(matrix *mat, vec3D p){
    matrix ptr, res_emb;
    matrix_embed_from_point(&ptr, p);

    matrix_product(mat, &ptr, &res_emb);

    matrix tmp;
    matrix_product_scalar(&res_emb, res_emb.array[3][0], &tmp);
    vec3D res = {tmp.array[0][0], tmp.array[1][0], tmp.array[2][0]};
    return res;
}


vec2i barycentric_to_world2i(vec2i *pts, vec3D bar){
    vec2i result;
    result.x = pts[0].x * bar.x + pts[1].x * bar.y + pts[2].x * bar.z;
    result.y = pts[0].y * bar.x + pts[1].y * bar.y + pts[2].y * bar.z;
    return result;
}



vec3i barycentric_to_world3i(vec3i *pts, vec3D bar){
    vec3i result;
    result.x = pts[0].x * bar.x + pts[1].x * bar.y + pts[2].x * bar.z;
    result.y = pts[0].y * bar.x + pts[1].y * bar.y + pts[2].y * bar.z;
    result.z = pts[0].z * bar.x + pts[1].z * bar.y + pts[2].z * bar.z;
    return result;
}

vec3D barycentric_to_world3D(vec3D *pts, vec3D bar){
    vec3D result;
    result.x = pts[0].x * bar.x + pts[1].x * bar.y + pts[2].x * bar.z;
    result.y = pts[0].y * bar.x + pts[1].y * bar.y + pts[2].y * bar.z;
    result.z = pts[0].z * bar.x + pts[1].z * bar.y + pts[2].z * bar.z;
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

void triangle(model *m, Shader *shader, vec3i pts[3], TGAImage &image, int *zbuffer){

    vec3i bboxmin, bboxmax;
    find_triangle_bounding_box(pts, &bboxmin, &bboxmax, image.get_width(), image.get_height());

    vec2i P;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
            vec3D bc_screen  = barycentric(pts, P);

            float frag_depth = bc_screen.x*pts[0].z + bc_screen.y*pts[1].z + bc_screen.z*pts[2].z;
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 
                    || zbuffer[P.x + P.y*image.get_width()] > frag_depth) continue;


            TGAColor color_diffuse;
            bool discard = shader->fragment_shader(shader, bc_screen, color_diffuse);

            if (!discard){
                zbuffer[P.x+P.y*image.get_width()] = frag_depth;
                image.set(P.x, P.y, color_diffuse);
            }
        }
    }
}



vec3i get_screen_coords(matrix *Transform, vec3D v){
    matrix V;
    matrix_initialize(&V, 4, 1);
    float array[4] = {v.x, v.y, v.z, 1};
    matrix_set(&V, array, 4);

    matrix result;
    
    matrix_product(Transform, &V, &result);

    vec3i res ={result.array[0][0] / result.array[3][0], 
                result.array[1][0] / result.array[3][0], 
                result.array[2][0] / result.array[3][0]};
    matrix_delete(&V);
    return res;
}
