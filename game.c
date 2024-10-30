#include "game.h"
#include <SDL.h>

static void handle_keydown(GameState * game_state, SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_A:
            game_state->rotate_yaw_left = true; break;
        case SDL_SCANCODE_D:
            game_state->rotate_yaw_right = true; break;
        case SDL_SCANCODE_W:
            game_state->rotate_pitch_up = true; break;
        case SDL_SCANCODE_S:
            game_state->rotate_pitch_down = true; break;
        case SDL_SCANCODE_LEFT:
            game_state->move_left = true; break;
        case SDL_SCANCODE_RIGHT:
            game_state->move_right = true; break;
        case SDL_SCANCODE_UP:
            game_state->move_up = true; break;
        case SDL_SCANCODE_DOWN:
            game_state->move_down = true; break;
        default:
    }
}

static void handle_keyup(GameState * game_state, SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_A:
            game_state->rotate_yaw_left = false; break;
        case SDL_SCANCODE_D:
            game_state->rotate_yaw_right = false; break;
        case SDL_SCANCODE_W:
            game_state->rotate_pitch_up = false; break;
        case SDL_SCANCODE_S:
            game_state->rotate_pitch_down = false; break;
        case SDL_SCANCODE_LEFT:
            game_state->move_left = false; break;
        case SDL_SCANCODE_RIGHT:
            game_state->move_right = false; break;
        case SDL_SCANCODE_UP:
            game_state->move_up = false; break;
        case SDL_SCANCODE_DOWN:
            game_state->move_down = false; break;
        default:
    }
}

void game_update(GameState * game_state) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT: game_state->running = false; break;
            case SDL_KEYDOWN:
                handle_keydown(game_state, e.key.keysym.scancode);
                break;
            case SDL_KEYUP:
                handle_keyup(game_state, e.key.keysym.scancode);
                break;
        }
        if (e.type == SDL_QUIT) {
            game_state->running = false;
            break;
        }
    }

    if (game_state->rotate_yaw_left) {
        game_state->yaw -= 0.1f;
    }

    if (game_state->rotate_yaw_right) {
        game_state->yaw += 0.1f;
    }

    if (game_state->rotate_pitch_up) {
        game_state->pitch -= 0.1f;
    }

    if (game_state->rotate_pitch_down) {
        game_state->pitch += 0.1f;
    }

    float speed = 10.0f;
    if (game_state->move_left) {
        game_state->offset.x -= speed;
    }
    if (game_state->move_right) {
        game_state->offset.x += speed;
    }
    if (game_state->move_up) {
        game_state->offset.y -= speed;
    }
    if (game_state->move_down) {
        game_state->offset.y += speed;
    }
}
