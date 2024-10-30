/*
    Radnyx 2024

    TODO:
    - actually project in 3d space and move the camera around
*/

#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include "render.h"
#include "game.h"

/**
 * Create a window to draw on.
 * \param out_window Returns the window.
 * \param out_renderer Returns the GPU accelerated renderer.
 */
static void setup(SDL_Window ** out_window, SDL_Renderer ** out_renderer) {
    SDL_Window * window = SDL_CreateWindow(
        "C Renderer", 
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (window == NULL) {
        printf("err: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("err: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        exit(1);
    }

    *out_window = window;
    *out_renderer = renderer;
}

/**
 * Clean up SDL resources.
 * \param window The window to destroy.
 * \param renderer The renderer to destroy.
 */
static void cleanup(SDL_Window * window, SDL_Renderer * renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char * argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("err: %s\n", SDL_GetError());
        return 1;
    }

    SDL_version version;
    SDL_GetVersion(&version);
    printf("SDL version: %d.%d.%d\n", version.major, version.minor, version.patch);

    SDL_Window * window;
    SDL_Renderer * renderer;
    setup(&window, &renderer);
   
    SDL_Texture * screen_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB4444, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    if (screen_texture == NULL) {
        printf("err: %s\n", SDL_GetError());
        cleanup(window, renderer);
        return 1;
    }


    const char * texture_filename = "assets/quake.png";
    const char * model_filename = "assets/quake.obj";

    SDL_Surface * image_surface = IMG_Load(texture_filename);
    if (image_surface == NULL) {
        printf("err: %s\n", SDL_GetError());
        SDL_DestroyTexture(screen_texture);
        cleanup(window, renderer);
        return 1;
    }

    printf("image w = %d, h = %d\n", image_surface->w, image_surface->h);
    printf("image bpp = %d, alpha = %x\n", image_surface->format->BitsPerPixel, image_surface->format->Amask);

    const Model * const model = model_load(model_filename);
    if (model == NULL) {
        printf("err: failed to load model: %s\n", model_filename);
        free(image_surface);
        SDL_DestroyTexture(screen_texture);
        cleanup(window, renderer);
        return 1;
    }

    printf("num_triangles = %d\n", model->num_triangles);

    static float t = 0;

    float * depth = malloc(WIDTH * HEIGHT * sizeof(float));
    memset(depth, 0, WIDTH * HEIGHT * sizeof(float));

    GameState game_state;
    memset(&game_state, 0, sizeof(GameState));
    game_state.offset = vec3_xy(model_center(model));
    game_state.scale = 1.0f / model_max_size(model);
    game_state.offset.x *= WIDTH / 2 * game_state.scale;
    game_state.offset.y *= HEIGHT / 2 * game_state.scale;
    printf("%f,%f", game_state.offset.x, game_state.offset.y);
    game_state.running = true;

    int fps = 0;
    uint32_t last_render_time = SDL_GetTicks();
    uint32_t last_update_time = SDL_GetTicks();
    while (game_state.running) {
        uint32_t current_time = SDL_GetTicks();
        if (current_time - last_update_time >= 16) {
            game_update(&game_state);
            last_update_time = current_time;
        }

        fps++;
        if (current_time - last_render_time >= 1000) {
            printf("FPS: %d\n", fps);
            fps = 0;
            last_render_time = SDL_GetTicks();
        }

        uint16_t * pixels;
        int _pitch;
        SDL_LockTexture(screen_texture, NULL, (void **) &pixels, &_pitch);
        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            pixels[i] = 0xF223; 
        }
        memset(depth, 0, WIDTH * HEIGHT * sizeof(float));

        RenderTexture render_texture = (RenderTexture) { 
            (uint32_t *) image_surface->pixels, 
            image_surface->w, 
            image_surface->h 
        };

        fill_model_flat(
            pixels, 
            depth, 
            model,
            &render_texture,
            game_state.offset,
            game_state.yaw,
            game_state.pitch,
            game_state.scale
        );

        /*t += 0.01;
        fill_triangle(
            pixels,
            depth,
            (Vec3) { -0.6, -1, 0 },
            (Vec3) { 0.6, 1, 0 },
            (Vec3) { 0.0 + cos(t) * 0.8, 0.0, 0 },
            (Vec2) { 0, 0 }, (Vec2) { 0, 0 }, (Vec2) { 0, 0 },
            NULL,
            0.0,
            (Vec2) { 0, 0 } 
        );*/

        SDL_UnlockTexture(screen_texture);


        // SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_UpdateWindowSurface(window);
    }

    free(image_surface);
    model_free(model);
    free(depth);

    SDL_DestroyTexture(screen_texture);
    cleanup(window, renderer);

    return 0;
}