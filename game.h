#ifndef GAME_H
#define GAME_H
#include <stdbool.h>
#include "vector.h"

/**
 * General read/write state about the currently active game.
 */
typedef struct {
    // View state
    Vec2 offset;
    float pitch;
    float yaw;
    float scale;

    // Controller state
    bool rotate_yaw_left, rotate_yaw_right;
    bool rotate_pitch_up, rotate_pitch_down;
    bool move_left, move_right, move_up, move_down;

    bool running;
} GameState;

/**
 * Update a single frame of game logic.
 * \param game_state The ever-changing state of the game.
 */
void game_update(GameState * game_state);

#endif // GAME_H