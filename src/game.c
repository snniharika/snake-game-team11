#include "snake.h"
#include <stdlib.h>

static bool same_position(Position a, Position b) {
    return a.x == b.x && a.y == b.y;
}

static bool snake_occupies(const GameState *game, Position p) {
    for (int i = 0; i < game->length; i++) {
        if (same_position(game->segments[i], p))
            return true;
    }
    return false;
}

/* SNK-F-007: Place food only on an unoccupied interior cell. */
static void spawn_food(GameState *game) {
    int free_count = 0;

    for (int y = 1; y < BOARD_HEIGHT - 1; y++) {
        for (int x = 1; x < BOARD_WIDTH - 1; x++) {
            Position p = {x, y};
            if (!snake_occupies(game, p))
                free_count++;
        }
    }

    if (free_count == 0) {
        game->game_over = true;
        return;
    }

    int target = rand() % free_count;

    for (int y = 1; y < BOARD_HEIGHT - 1; y++) {
        for (int x = 1; x < BOARD_WIDTH - 1; x++) {
            Position p = {x, y};

            if (!snake_occupies(game, p)) {
                if (target == 0) {
                    game->food = p;
                    return;
                }
                target--;
            }
        }
    }
}

/* SNK-F-003: Initialize the board, snake, direction and score. */
void initialize_game(GameState *game, Difficulty difficulty) {
    game->length = INITIAL_LENGTH;
    game->score = 0;
    game->direction = DIR_RIGHT;
    game->difficulty = difficulty;
    game->game_over = false;

    int center_x = BOARD_WIDTH / 2;
    int center_y = BOARD_HEIGHT / 2;

    game->segments[0] = (Position){center_x, center_y};
    game->segments[1] = (Position){center_x - 1, center_y};
    game->segments[2] = (Position){center_x - 2, center_y};

    /* Temporary food position; Issue 7 will implement safe spawning. */
    game->food.x = BOARD_WIDTH / 2;
    game->food.y = BOARD_HEIGHT / 2 - 2;
}

/* SNK-F-004: Ignore direct 180-degree reversals. */
void update_direction(GameState *game, Key key) {
    Direction requested = game->direction;

    switch (key) {
        case KEY_UP:    requested = DIR_UP; break;
        case KEY_DOWN:  requested = DIR_DOWN; break;
        case KEY_LEFT:  requested = DIR_LEFT; break;
        case KEY_RIGHT: requested = DIR_RIGHT; break;
        default: return;
    }

    bool opposite =
        (game->direction == DIR_UP && requested == DIR_DOWN) ||
        (game->direction == DIR_DOWN && requested == DIR_UP) ||
        (game->direction == DIR_LEFT && requested == DIR_RIGHT) ||
        (game->direction == DIR_RIGHT && requested == DIR_LEFT);

    if (!opposite)
        game->direction = requested;
}

void update_game(GameState *game) {
    Position next = game->segments[0];

    switch (game->direction) {
        case DIR_UP:    next.y--; break;
        case DIR_DOWN:  next.y++; break;
        case DIR_LEFT:  next.x--; break;
        case DIR_RIGHT: next.x++; break;
    }

    bool eating = same_position(next, game->food);

    // Keep the snake inside the board.
    if (next.x <= 0 || next.x >= BOARD_WIDTH - 1 ||
        next.y <= 0 || next.y >= BOARD_HEIGHT - 1) {
        game->game_over = true;
        return;
    }

    // The tail vacates its cell on a non-eating move.
    int body_limit = eating ? game->length : game->length - 1;

    for (int i = 0; i < body_limit; i++) {
        if (same_position(next, game->segments[i])) {
            game->game_over = true;
            return;
        }
    }

    if (eating && game->length >= BOARD_WIDTH * BOARD_HEIGHT - 4) {
        game->game_over = true;
        return;
    }

    if (eating)
        game->length++;

    for (int i = game->length - 1; i > 0; i--)
        game->segments[i] = game->segments[i - 1];

    game->segments[0] = next;

    if (eating) {
        game->score++;
        spawn_food(game);
    }
}
