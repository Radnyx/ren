#include "vector.h"
#include <stdio.h>

Vec2 vec2_add(Vec2 a, Vec2 b) {
    return (Vec2) { a.x + b.x, a.y + b.y };
}

Vec2 vec2_scale(Vec2 a, float t) {
    return (Vec2) { a.x * t, a.y * t };
}

Vec2 vec2_mul(Vec2 a, Vec2 b) {
    return (Vec2) { a.x * b.x, a.y * b.y };
}

void vec2_swap(Vec2 * a, Vec2 * b) {
    Vec2 temp = *a;
    *a = *b;
    *b = temp;
}

void vec3_swap(Vec3 * a, Vec3 * b) {
    Vec3 temp = *a;
    *a = *b;
    *b = temp;
}

bool vec2_eq(Vec2 a, Vec2 b) {
    return a.x == b.x && a.y == b.y;
}


float vec3_len(Vec3 a) {
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

float vec3_dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 vec3_cross(Vec3 a, Vec3 b) {
    return (Vec3) {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

Vec3 vec3_scale(Vec3 a, float t) {
    return (Vec3) {
        a.x * t,
        a.y * t,
        a.z * t
    };
}

Vec3 vec3_add(Vec3 a, Vec3 b) {
    return (Vec3) {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
    return (Vec3) {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

Vec2 vec3_xy(Vec3 a) {
    return (Vec2) { a.x, a.y };
}


void print_vec2(char * s, Vec2 a) {
    printf("%s (%.2f, %.2f)\n", s, a.x, a.y);
}

void print_vec3(char * s, Vec3 a) {
    printf("%s (%.2f, %.2f, %.2f)\n", s, a.x, a.y, a.z);
}
