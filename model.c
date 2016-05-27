#include "model.h"
#include "geometry.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void model_add_vert(model *m, vec3D v){
    m->verts[(m->vn)++] = v;
}

void model_add_face(model *m, int i1, int i2, int i3){
    m->faces[(m->fn)][0]    = i1;
    m->faces[(m->fn)][1]    = i2;
    m->faces[(m->fn++)][2]  = i3;
}

void model_print(model *m){
    printf("verts:\n");
    for (int i = 0; i < m->vn; i++){
        printf("%f\t%f\t%f\n", m->verts[i].x, m->verts[i].y, m->verts[i].z);
    }

    printf("\nfaces:\n");
    for (int i = 0; i < m->fn; i++){
        printf("%d\t%d\t%d\n", m->faces[i][0], m->faces[i][1], m->faces[i][2]);
    }

    printf("\n");
}

void model_load(model *m, char *filename){
    FILE *f = fopen(filename, "r");
    if (f == 0){
        printf("ERROR in open file");
        return;
    }

    m->fn = 0;
    m->vn = 0;

    char line[256];

    while(fgets(line, 256, f)){
        if (!strncmp(line, "v ", 2)){
            vec3D v;
            char s[2];
            sscanf(line, "%s %f %f %f", s, &v.x, &v.y, &v.z);
            // printf("v");

            model_add_vert(m, v);
        } else if (!strncmp(line, "f ", 2)){
            char s[2], s1[128], s2[128], s3[128];

            sscanf(line, "%s %s %s %s", s, s1, s2, s3);
            int i1, i2, i3;
            i1 = atoi(strtok(s1, "/"));
            i2 = atoi(strtok(s2, "/"));
            i3 = atoi(strtok(s3, "/"));

            // printf("f");
            model_add_face(m, i1-1, i2-1, i3-1);
        } else {
            // printf("n");
        }
    }
}