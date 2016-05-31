#include "model.h"
#include "geometry.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void model_add_norm(model *m, vec3D n){
    m->norms[(m->nn)++] = n;
}

void model_add_uv(model *m, vec3D uv){
    m->uv[(m->un)++] = uv;    
}

void model_add_vert(model *m, vec3D v){
    m->verts[(m->vn)++] = v;
}

void model_add_face(model *m, char line[256]){
    char s[3], s1[128], s2[128], s3[128];

    sscanf(line, "%s %s %s %s", s, s1, s2, s3);
    int i1, i2, i3;
    i1 = atoi(strtok(s1, "/"));
    i2 = atoi(strtok(NULL, "/"));
    i3 = atoi(strtok(NULL, "/"));
    m->faces[(m->fn)][0][0] = i1-1;
    m->faces[(m->fn)][1][0] = i2-1;
    m->faces[(m->fn)][2][0] = i3;

    i1 = atoi(strtok(s2, "/"));
    i2 = atoi(strtok(NULL, "/"));
    i3 = atoi(strtok(NULL, "/"));
    m->faces[(m->fn)][0][1] = i1-1;
    m->faces[(m->fn)][1][1] = i2-1;
    m->faces[(m->fn)][2][1] = i3;

    i1 = atoi(strtok(s3, "/"));
    i2 = atoi(strtok(NULL, "/"));
    i3 = atoi(strtok(NULL, "/"));
    m->faces[(m->fn)][0][2] = i1-1;
    m->faces[(m->fn)][1][2] = i2-1;
    m->faces[(m->fn++)][2][2] = i3;
}

// void model_print(model *m){
//     printf("verts:\n");
//     for (int i = 0; i < m->vn; i++){
//         printf("%f\t%f\t%f\n", m->verts[i].x, m->verts[i].y, m->verts[i].z);
//     }

//     printf("\nfaces:\n");
//     for (int i = 0; i < m->fn; i++){
//         printf("%d\t%d\t%d\n", m->faces[i][0], m->faces[i][1], m->faces[i][2]);
//     }

//     printf("\n");
// }

void model_load_texture(char *filename, const char *suffix, TGAImage &img) {
    std::string texfile(filename);
    size_t dot = texfile.find_last_of(".");
    if (dot!=std::string::npos) {
        texfile = texfile.substr(0,dot) + std::string(suffix);
        if (!img.read_tga_file(texfile.c_str())){
            printf("ERROR in texture loading\n");
        }
        // std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
        img.flip_vertically();
    }
}

TGAColor model_diffuse(model *m, vec2i uv) {
    return m->diffusemap.get(uv.x, uv.y);
}

vec2i model_uv(model *m, int iface, int nvert) {
    int idx = m->faces[iface][1][nvert];
    vec2i result = {(int)(m->uv[idx].x*m->diffusemap.get_width()), (int)(m->uv[idx].y*m->diffusemap.get_height())};
    return result;
}


void model_load(model *m, char *filename){
    FILE *f = fopen(filename, "r");
    if (f == 0){
        printf("ERROR in open file");
        return;
    }

    m->fn = 0;
    m->vn = 0;
    m->nn = 0;
    m->un = 0;

    char line[256];

    while(fgets(line, 256, f)){
        char s[8];
        if (!strncmp(line, "v ", 2)){
            vec3D v;
            sscanf(line, "%s %f %f %f", s, &v.x, &v.y, &v.z);
            // printf("v");

            model_add_vert(m, v);
        } else if (!strncmp(line, "f ", 2)){
            

            // printf("f");
            model_add_face(m, line);
        } else if (!strncmp(line, "vn ", 3)){
            vec3D n;
            sscanf(line, "%s %f %f %f", s, &n.x, &n.y, &n.z);

            model_add_norm(m, n);
        } else if (!strncmp(line, "vt ", 3)){
            vec3D uv;
            uv.z = 0;
            sscanf(line, "%s %f %f", s, &uv.x, &uv.y);

            model_add_uv(m, uv);
        }
    }

    model_load_texture(filename, "_diffuse.tga", m->diffusemap);
}