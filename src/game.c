
#include "snake.h"
#include <stdlib.h>

/*
 * SNK-SR-001: Validate game state before processing updates.
 */
static bool valid_game_state(const GameState *game) {
    if (game == NULL)
        return false;

    if (game->length < 1 ||
        game->length > BOARD_WIDTH * BOARD_HEIGHT)
        return false;

    if (game->score < 0)
        return false;

    if (game->direction < DIR_UP ||
        game->direction > DIR_RIGHT)
        return false;

    if (game->difficulty < EASY ||
        game->difficulty > HARD)
        return false;

    if (game->food.x < 1 || game->food.x >= BOARD_WIDTH - 1 ||
        game->food.y < 1 || game->food.y >= BOARD_HEIGHT - 1)
        return false;

    for (int i = 0; i < game->length; i++) {
        if (game->segments[i].x < 1 ||
            game->segments[i].x >= BOARD_WIDTH - 1 ||
            game->segments[i].y < 1 ||
            game->segments[i].y >= BOARD_HEIGHT - 1)
            return false;
    }

    return true;
}

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

/* SNK-F-003: Initialize the snake and score. */
void initialize_game(GameState *game, Difficulty difficulty) {
    if (game == NULL || difficulty < EASY || difficulty > HARD)
        return;

    game->length = INITIAL_LENGTH;
    game->score = 0;
    game->direction = DIR_RIGHT;
    game->difficulty = difficulty;
    game->game_over = false;

    int cx = BOARD_WIDTH / 2;
    int cy = BOARD_HEIGHT / 2;

    game->segments[0] = (Position){cx, cy};
    game->segments[1] = (Position){cx - 1, cy};
    game->segments[2] = (Position){cx - 2, cy};

    spawn_food(game);
}

/* SNK-F-004: Ignore direct 180-degree reversals. */
void update_direction(GameState *game, Key key) {
    if (!valid_game_state(game) || game->game_over)
        return;

    Direction requested = game->direction;

    switch (key) {
        case KEY_UP:    requested = DIR_UP; break;
        case KEY_DOWN:  requested = DIR_DOWN; break;
        case KEY_LEFT:  requested = DIR_LEFT; break;
        case KEY_RIGHT: requested = DIR_RIGHT; break;
        case KEY_NONE:
        case KEY_ENTER:
        case KEY_ESCAPE:
        case KEY_QUIT:
        default:
            return;
    }

    bool opposite =
        (game->direction == DIR_UP && requested == DIR_DOWN) ||
        (game->direction == DIR_DOWN && requested == DIR_UP) ||
        (game->direction == DIR_LEFT && requested == DIR_RIGHT) ||
        (game->direction == DIR_RIGHT && requested == DIR_LEFT);

    if (!opposite)
        game->direction = requested;
}

/*
 * SNK-SR-002: Keep score changes inside the validated game-update
 * flow. A point is awarded only when the snake reaches the food.
 * No public function accepts an arbitrary score value.
 */
void update_game(GameState *game) {
    if (!valid_game_state(game) || game->game_over)
        return;

    Position next = game->segments[0];

    switch (game->direction) {
        case DIR_UP:    next.y--; break;
        case DIR_DOWN:  next.y++; break;
        case DIR_LEFT:  next.x--; break;
        case DIR_RIGHT: next.x++; break;
        default:
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
        if (game->score < 0 || game->score == 2147483647) {
            game->game_over = true;
            return;
        }

        game->score++;
        spawn_food(game);
    }
}
