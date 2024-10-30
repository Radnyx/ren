#ifndef RENDER_H
#define RENDER_H
#include <SDL.h>
#include <stdint.h>
#include "vector.h"
#include "model.h"

// TODO: MAKE SOME KIND OF CONTEXT

#define WIDTH 640
#define HEIGHT 480

typedef struct {
    uint32_t * pixels;
    int w, h;
} RenderTexture;

typedef uint16_t color_t;

void draw_line(uint16_t * pixels, int x0, int y0, int x1, int y1, color_t color);

void fill_triangle(
    uint16_t * pixels,
    float * depth,
    Vec3 v1_world, Vec3 v2_world, Vec3 v3_world, 
    Vec2 uv1, Vec2 uv2, Vec2 uv3,
    const RenderTexture * texture,
    float amount,
    Vec2 offset
);

void draw_wireframe(uint16_t * pixels, Model * model);
void fill_model_flat(
    uint16_t * pixels,
    float * depth,
    Model * model,
    const RenderTexture * texture,
    Vec2 offset,
    float yaw,
    float pitch,
    float scale    
);

#endif // RENDER_H