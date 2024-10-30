#include "render.h"
#include <stdlib.h>
#include <math.h>

void draw_line(uint16_t * pixels, int x0, int y0, int x1, int y1, color_t color) {
    if (y0 > y1) {
        int temp = y0;
        y0 = y1;
        y1 = temp;
        temp = x0;
        x0 = x1;
        x1 = temp;
    } else if (y0 == y1) {
        if (x0 > x1) {
            int temp = x0;
            x0 = x1;
            x1 = temp;
        }
    
        for (int x = x0; x <= x1; x++) {
            pixels[y0 * WIDTH + x] = color;
        }
        return;
    }

    float inv_m = (float)(x1 - x0) / (y1 - y0);

    int start_y = SDL_max(0, y0);
    int end_y = SDL_min(HEIGHT - 1, y1);
    for (int y = start_y; y < end_y; y++) {
        // compute where x should be at y and y + 1
        int start_x = x0 + inv_m * (y - y0);
        int end_x = x0 + inv_m * (y + 1 - y0);
    
        if (start_x > end_x) {
            int temp = start_x;
            start_x = end_x;
            end_x = temp;
        }

        start_x = SDL_max(0, start_x);
        end_x = SDL_min(WIDTH - 1, end_x);
        for (int x = start_x; x <= end_x; x++) {
            pixels[y * WIDTH + x] = color;
        }
    }
}

static Vec3 rotate_xz(Vec3 v, float angle) {
    return (Vec3) {
        v.x * cosf(angle) - v.z * sinf(angle),
        v.y,
        v.x * sinf(angle) + v.z * cosf(angle)
    };
}

static Vec3 rotate_yz(Vec3 v, float angle) {
    return (Vec3) {
        v.x,
        v.y * cosf(angle) - v.z * sinf(angle),
        v.y * sinf(angle) + v.z * cosf(angle)
    };
}

void draw_wireframe(uint16_t * pixels, Model * model) {
    static float angle = 0.0;
    angle += 0.001;

    Vec2 half_screen_size = { WIDTH / 2, HEIGHT / 2 };
    for (int i = 0; i < model->num_triangles; i++) {
        Triangle t = model->triangles[i];

        Vec3 mv1 = model->vertices[t.v1];
        Vec3 mv2 = model->vertices[t.v2];
        Vec3 mv3 = model->vertices[t.v3];

        mv1 = rotate_xz(mv1, angle);
        mv2 = rotate_xz(mv2, angle);
        mv3 = rotate_xz(mv3, angle);

        Vec2 v1 = { mv1.x, -mv1.y };
        Vec2 v2 = { mv2.x, -mv2.y };
        Vec2 v3 = { mv3.x, -mv3.y };

        v1 = vec2_mul(vec2_add(v1, (Vec2) { 1, 1 }), half_screen_size);
        v2 = vec2_mul(vec2_add(v2, (Vec2) { 1, 1 }), half_screen_size);
        v3 = vec2_mul(vec2_add(v3, (Vec2) { 1, 1 }), half_screen_size);
        
        draw_line(pixels, v1.x, v1.y, v2.x, v2.y, 0xFFFF);
        draw_line(pixels, v2.x, v2.y, v3.x, v3.y, 0xFFFF);
        draw_line(pixels, v3.x, v3.y, v1.x, v1.y, 0xFFFF);
    }
}

static float clamp(float x, float mn, float mx) {
    x = SDL_max(mn, x);
    x = SDL_min(mx, x);
    return x;
}

void fill_model_flat(
    uint16_t * pixels,
    float * depth,
    Model * model,
    const RenderTexture * texture,
    Vec2 offset,
    float yaw,
    float pitch,
    float scale
) {
    Vec3 light = (Vec3) { 0.0, 0.0, -1.0 };

    Vec2 half_screen_size = { WIDTH / 2, HEIGHT / 2 };
    // for (int i = 100; i < 120; i++) {
    for (int i = 0; i < model->num_triangles; i++) {
        Triangle t = model->triangles[i];
        Vec3 mv1 = vec3_scale(model->vertices[t.v1], scale);
        Vec3 mv2 = vec3_scale(model->vertices[t.v2], scale);
        Vec3 mv3 = vec3_scale(model->vertices[t.v3], scale);

        mv1 = rotate_yz(rotate_xz(mv1, yaw), pitch);
        mv2 = rotate_yz(rotate_xz(mv2, yaw), pitch);
        mv3 = rotate_yz(rotate_xz(mv3, yaw), pitch);

        Vec3 normal = vec3_cross(vec3_sub(mv2, mv1), vec3_sub(mv3, mv1));
        float amount = -vec3_dot(normal, light) / (vec3_len(normal) * vec3_len(light));
        // amount = SDL_min(1.0f, amount * 2.0f);
        if (amount < 0) {
            continue;
        }

        Vec2 uv1 = model->uvs[t.vt1];
        Vec2 uv2 = model->uvs[t.vt2];
        Vec2 uv3 = model->uvs[t.vt3];

        fill_triangle(
            pixels, depth, 
            mv1, mv2, mv3, 
            uv1, uv2, uv3,
            texture, amount, 
            offset
        );
    }
}

void sort_vertices_by_y(
    Vec2 * v1, Vec2 * v2, Vec2 * v3,
    Vec3 * v1_world, Vec3 * v2_world, Vec3 * v3_world,
    Vec2 * uv1, Vec2 * uv2, Vec2 * uv3
) {
    // swap v2 and v3
    if (v2->y > v3->y) {
        vec2_swap(v2, v3);
        vec3_swap(v2_world, v3_world);
        vec2_swap(uv2, uv3);
    }

    // swap v1 and v2
    if (v1->y > v2->y) {
        vec2_swap(v1, v2);
        vec3_swap(v1_world, v2_world);
        vec2_swap(uv1, uv2);
    }
    
    // swap v2 and v3
    if (v2->y > v3->y) {
        vec2_swap(v2, v3);
        vec3_swap(v2_world, v3_world);
        vec2_swap(uv2, uv3);
    }
}

void fill_triangle(
    uint16_t * pixels,
    float * depth,
    Vec3 v1_world, Vec3 v2_world, Vec3 v3_world, 
    Vec2 uv1, Vec2 uv2, Vec2 uv3,
    const RenderTexture * texture,
    float amount,
    Vec2 offset
) {
    // 1. project triangle onto screen
    Vec2 v1 = { v1_world.x, -v1_world.y };
    Vec2 v2 = { v2_world.x, -v2_world.y };
    Vec2 v3 = { v3_world.x, -v3_world.y };

    Vec2 half_screen_size = { WIDTH / 2, HEIGHT / 2 };
    v1 = vec2_mul(vec2_add(v1, (Vec2) { 1, 1 }), half_screen_size);
    v2 = vec2_mul(vec2_add(v2, (Vec2) { 1, 1 }), half_screen_size);
    v3 = vec2_mul(vec2_add(v3, (Vec2) { 1, 1 }), half_screen_size);

    v1 = vec2_add(offset, v1);
    v2 = vec2_add(offset, v2);
    v3 = vec2_add(offset, v3);

    // 2. fill triangle
    if (
        (v1.x == v2.x && v2.x == v3.x) ||
        (v1.y == v2.y && v2.y == v3.y) ||
        vec2_eq(v1, v2) ||
        vec2_eq(v2, v3) ||
        vec2_eq(v3, v1)
    ) {
        return;
    }
    
    sort_vertices_by_y(
        &v1, &v2, &v3,
        &v1_world, &v2_world, &v3_world,
        &uv1, &uv2, &uv3
    );

    float inv_m = (v3.x - v1.x) / (v3.y - v1.y);
    float inv_m_leg1 = (v2.x - v1.x) / (v2.y - v1.y);
    float inv_m_leg2 = (v3.x - v2.x) / (v3.y - v2.y);

    int start_y = roundf(v1.y);
    int mid_y = roundf(v2.y);
    int end_y = roundf(v3.y);
    int clamp_start_y = SDL_max(0, start_y);
    int clamp_end_y = SDL_min(HEIGHT - 1, end_y);

    for (int y = clamp_start_y; y < clamp_end_y; y++) {
        float start_x = v1.x + (y - start_y) * inv_m;
        float end_x;

        Vec3 bary2;
        if (y < mid_y) {
            end_x = v1.x + (y - start_y) * inv_m_leg1;

            float t2 = (float) (y - start_y) / (mid_y - start_y);
            bary2 = (Vec3) { 1 - t2, t2, 0 };
        } else {
            end_x = v2.x + (y - mid_y) * inv_m_leg2;

            float t2 = (float) (y - mid_y) / (end_y - mid_y);
            bary2 = (Vec3) { 0, 1 - t2, t2 };
        }

        bool left = start_x > end_x;

        if (left) {
            float temp = start_x;
            start_x = end_x;
            end_x = temp;
        }

        float t1 = (float) (y - start_y) / (end_y - start_y);
        Vec3 bary1 = (Vec3) { 1 - t1, 0, t1 };

        int clamp_start_x = roundf(SDL_max(0, start_x));
        int clamp_end_x = roundf(SDL_min(WIDTH - 1, end_x));
        for (int x = clamp_start_x; x <= clamp_end_x; x++) {
            float t3 = (x - start_x) / (end_x - start_x);

            Vec3 bary;
            if (!left) {
                bary = vec3_add(vec3_scale(bary1, 1 - t3), vec3_scale(bary2, t3));
            } else {
                bary = vec3_add(vec3_scale(bary1, t3), vec3_scale(bary2, 1 - t3));
            }

            Vec3 p_world = vec3_add(
                vec3_add(
                    vec3_scale(v1_world, bary.x),
                    vec3_scale(v2_world, bary.y)
                ),
                vec3_scale(v3_world, bary.z)
            );

            p_world.z += 10000.0;
            if (p_world.z > depth[y * WIDTH + x]) {
                Vec2 uv = vec2_add(
                    vec2_add(
                        vec2_scale(uv1, bary.x),
                        vec2_scale(uv2, bary.y)
                    ),
                    vec2_scale(uv3, bary.z)
                );

                uint8_t a, r, g, b;
                if (texture != NULL) {
                    uv.x *= texture->w;
                    uv.y *= texture->h;
                    int u = (int)uv.x;
                    int v = texture->h - (int)uv.y - 1;
                    if (u < 0 || v < 0 || u >= texture->w || v >= texture->h) {
                        printf("%d %d\n", u, v);
                        break;
                    }

                    uint32_t texture_col = texture->pixels[v * texture->w + u];
                    a = (texture_col >> 24) >> 4;
                    b = ((texture_col >> 16) & 0xFF) >> 4;
                    g = ((texture_col >> 8) & 0xFF) >> 4;
                    r = ((texture_col) & 0xFF) >> 4;
                } else {
                    a = 0xF;
                    r = 0xF;
                    g = 0xF;
                    b = 0xF;
                }
                r = SDL_min((int)(r * amount) + 1, 0xF);
                g = SDL_min((int)(g * amount) + 1, 0xF);
                b = SDL_min((int)(b * amount) + 2, 0xF);
                
                /*pixels[y * WIDTH + x] = 0xF000 |
                    ((int)(bary.x * 0xF) << 8) |
                    ((int)(bary.y * 0xF) << 4) |
                    (int)(bary.z * 0xF);*/
                pixels[y * WIDTH + x] = (a << 12) | (r << 8) | (g << 4) | b;
                depth[y * WIDTH + x] = p_world.z;
            }
        }
    }
}
