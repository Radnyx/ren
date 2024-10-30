#include "model.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL.h>
#include <math.h>

Triangle * push_triangle(Triangle * triangles, Triangle tri, int * count, int * capacity) {
    triangles[(*count)++] = tri;
    if (*count >= *capacity) {
        *capacity *= 2;
        triangles = realloc(triangles, sizeof(Triangle) * *capacity);
    }
    return triangles;
}

const Model * const model_load(const char * filename) {
    FILE * file = fopen(filename, "r");
    if (file == NULL) {
        printf("err: could not open file: %s\n", filename);
        return NULL;
    }

    Model * model = malloc(sizeof(Model));
    model->num_vertices = 0;
    model->num_uvs = 0;
    model->num_triangles = 0;

    int vertices_capacity = 1024;
    Vec3 * vertices = malloc(sizeof(Vec3) * vertices_capacity);

    int uvs_capacity = 1024;
    Vec2 * uvs = malloc(sizeof(Vec2) * uvs_capacity);

    int triangles_capacity = 1024;
    Triangle * triangles = malloc(sizeof(Triangle) * triangles_capacity);

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        size_t len = strlen(line);
        if (len < 2) continue;

        // printf(line);

        // add a vertex
        if (line[0] == 'v' && line[1] == ' ') {
            float x, y, z;

            if (sscanf(line, "v %f %f %f", &x, &y, &z) == EOF) {
                free(triangles);
                free(uvs);
                free(vertices);
                free(model);
                return NULL;
            }

            vertices[model->num_vertices] = (Vec3) { x, y, z };
            model->num_vertices++;

            if (model->num_vertices >= vertices_capacity) {
                vertices_capacity *= 2;
                vertices = realloc(vertices, sizeof(Vec3) * vertices_capacity);
            }
        } 
        // add a uv
        else if (line[0] == 'v' && line[1] == 't') {
            float u, v, w;
            if (sscanf(line, "vt %f %f %f", &u, &v, &w) == EOF) {
                free(triangles);
                free(uvs);
                free(vertices);
                free(model);
                return NULL;
            }

            uvs[model->num_uvs++] = (Vec2) { u, v };
            if (model->num_uvs >= uvs_capacity) {
                uvs_capacity *= 2;
                uvs = realloc(uvs, sizeof(Vec2) * uvs_capacity);
            }
        }
        // add a face
        else if (line[0] == 'f' && line[1] == ' ') {
            int v1, v2, v3, v4;
            int vt1, vt2, vt3, vt4;
            int vn1, vn2, vn3, vn4; // TODO: ignoring vertex normals for now

            int result = sscanf(
                line, 
                "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", 
                &v1, &vt1, &vn1,
                &v2, &vt2, &vn2,
                &v3, &vt3, &vn3,
                &v4, &vt4, &vn4
            );

            if (result <= 2) {
               result = sscanf(
                    line, 
                    "f %d/%d %d/%d %d/%d", 
                    &v1, &vt1,
                    &v2, &vt2,
                    &v3, &vt3
                );
            }

            if (result == EOF) {
                free(triangles);
                free(uvs);
                free(vertices);
                free(model);
                return NULL;
            }
            // quad time
            /* else if (result == 8) {
                triangles = push_triangle(
                    triangles, 
                    (Triangle) { v1 - 1, v2 - 1, v3 - 1 },
                    &model->num_triangles,
                    &triangles_capacity
                );
                triangles = push_triangle(
                    triangles,
                    (Triangle) { v1 - 1, v3 - 1, v4 - 1 },
                    &model->num_triangles,
                    &triangles_capacity
                );
            } */
            // tri time
            else {
                triangles = push_triangle(
                    triangles, 
                    (Triangle) { v1 - 1, v2 - 1, v3 - 1, vt1 - 1, vt2 - 1, vt3 - 1 },
                    &model->num_triangles,
                    &triangles_capacity
                );
            }
        }
    }

    fclose(file);

    model->vertices = vertices;
    model->uvs = uvs;
    model->triangles = triangles;

    return model;
}

void model_free(const Model * model) {
    free(model->vertices);
    free(model->uvs);
    free(model->triangles);
    free(model);
    return 0;
}

Vec3 model_center(const Model * model) {
    Vec3 avg = (Vec3) { 0, 0, 0 };
    if (model != NULL) {
        for (int i = 0; i < model->num_vertices; i++) {
            avg = vec3_add(avg, model->vertices[i]);
        }

        avg = vec3_scale(avg, 1.0 / model->num_vertices);
    }
    return avg;
}

float model_max_size(const Model * model) {
    if (model == NULL) return -1;

    Vec3 center = model_center(model);

    float maximum = 0.0;
    for (int i = 0; i < model->num_vertices; i++) {
        Vec3 vertex = model->vertices[i];
        maximum = SDL_max(maximum, fabsf(vertex.x - center.x));
        maximum = SDL_max(maximum, fabsf(vertex.y - center.y));
        maximum = SDL_max(maximum, fabsf(vertex.z - center.z));
    }

    return maximum;
}