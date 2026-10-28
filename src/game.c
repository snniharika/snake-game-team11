
#include "snake.h"
#include <stdlib.h>
#include <limits.h>

static bool same_position(Position a, Position b) {
    return a.x == b.x && a.y == b.y;
}

static bool snake_occupies(const GameState *game, Position p) {
    if (game == NULL || game->length < 0 ||
        game->length > BOARD_WIDTH * BOARD_HEIGHT)
        return false;

    for (int i = 0; i < game->length; i++) {
        if (same_position(game->segments[i], p))
            return true;
    }

    return false;
}

/* SNK-F-007: Place food only on an unoccupied interior cell. */
static void spawn_food(GameState *game) {
    if (game == NULL)
        return;

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

/* SNK-F-003: Initialize the snake and score. */
void initialize_game(GameState *game, Difficulty difficulty) {
    if (game == NULL)
        return;

    if (difficulty < EASY || difficulty > HARD)
        difficulty = EASY;

    game->length = INITIAL_LENGTH;
    game->score = 0;
    game->direction = DIR_RIGHT;
    game->difficulty = difficulty;
    game->game_over = false;
    game->paused = false;

    int cx = BOARD_WIDTH / 2;
    int cy = BOARD_HEIGHT / 2;

    game->segments[0] = (Position){cx, cy};
    game->segments[1] = (Position){cx - 1, cy};
    game->segments[2] = (Position){cx - 2, cy};

    spawn_food(game);
}

/* SNK-F-004: Ignore direct 180-degree reversals. */
void update_direction(GameState *game, Key key) {
    if (game == NULL || game->paused || game->game_over)
        return;

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

/* SNK-F-013: Pause or resume only an active game. */
void set_paused(GameState *game, bool paused) {
    if (game == NULL || game->game_over)
        return;

    game->paused = paused;
}

void update_game(GameState *game) {
    if (game == NULL || game->paused || game->game_over)
        return;

    if (game->length < 1 ||
        game->length > BOARD_WIDTH * BOARD_HEIGHT - 4) {
        game->game_over = true;
        return;
    }

    if (game->score < 0) {
        game->game_over = true;
        return;
    }

    Position next = game->segments[0];

    switch (game->direction) {
        case DIR_UP:    next.y--; break;
        case DIR_DOWN:  next.y++; break;
        case DIR_LEFT:  next.x--; break;
        case DIR_RIGHT: next.x++; break;
        default:
            game->game_over = true;
            return;
    }

    bool eating = same_position(next, game->food);

    if (next.x <= 0 || next.x >= BOARD_WIDTH - 1 ||
        next.y <= 0 || next.y >= BOARD_HEIGHT - 1) {
        game->game_over = true;
        return;
    }

    int body_limit = eating ? game->length : game->length - 1;

    for (int i = 0; i < body_limit; i++) {
        if (same_position(next, game->segments[i])) {
            game->game_over = true;
            return;
        }
    }

    if (eating &&
        (game->length >= BOARD_WIDTH * BOARD_HEIGHT - 4 ||
         game->score == INT_MAX)) {
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
