#ifndef VECTOR_H
#define VECTOR_H
#include <stdbool.h>

typedef struct {
    float x, y;
} Vec2;

typedef struct {
    float x, y, z;
} Vec3;

Vec2 vec2_scale(Vec2 a, float t);
Vec2 vec2_add(Vec2 a, Vec2 b);
Vec2 vec2_mul(Vec2 a, Vec2 b);

void vec2_swap(Vec2 * a, Vec2 * b);
bool vec2_eq(Vec2 a, Vec2 b);


void vec3_swap(Vec3 * a, Vec3 * b);
float vec3_len(Vec3 a);
float vec3_dot(Vec3 a, Vec3 b);
Vec3 vec3_cross(Vec3 a, Vec3 b);

Vec3 vec3_scale(Vec3 a, float t);
Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_sub(Vec3 a, Vec3 b);
Vec2 vec3_xy(Vec3 a);


void print_vec2(char * s, Vec2 a);
void print_vec3(char * s, Vec3 a);

#endif // VECTOR_H