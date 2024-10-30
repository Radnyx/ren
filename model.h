#ifndef MODEL_H
#define MODEL_H
#include "vector.h"

typedef struct {
    int v1, v2, v3;
    int vt1, vt2, vt3;
} Triangle;

typedef struct {
    int num_vertices;
    Vec3 * vertices;

    int num_uvs;
    Vec2 * uvs;

    int num_triangles;
    Triangle * triangles;
} Model;

/**
 * Load a model from a .obj file.
 * \param filename The file to load.
 * \returns `NULL` if the file doesn't exist or is malformed.
 */
const Model * const model_load(const char * filename);

/**
 * \param model The model to free along with its allocated buffers.
 */
void model_free(const Model * model);

/**
 * \param model Model to analyze.
 * \returns The center point of the model, being the average of all points. Zero vector if the model is `NULL`.
 */
Vec3 model_center(const Model * model);

/**
 * \param model Model to analyze.
 * \returns The largest component-wise distance away from the center, `-1` if the model is `NULL`.
 */
float model_max_size(const Model * model);

#endif // MODEL_H