#ifndef SNAKE_H
#define SNAKE_H

#include <stdbool.h>

#define BOARD_WIDTH 30
#define BOARD_HEIGHT 15
#define INITIAL_LENGTH 3

typedef struct {
    int x;
    int y;
} Position;

typedef enum {
    DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT
} Direction;

typedef enum {
    EASY, MEDIUM, HARD
} Difficulty;

typedef enum {
    KEY_NONE, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    KEY_ENTER, KEY_ESCAPE, KEY_QUIT
} Key;

typedef struct {
    Position segments[BOARD_WIDTH * BOARD_HEIGHT];
    int length;
    Position food;
    Direction direction;
    Difficulty difficulty;
    int score;
    bool game_over;
} GameState;

void initialize_input(void);
void shutdown_input(void);
Key read_key(void);

void render_menu(int selected, Difficulty difficulty);
void render_game(const GameState *game);

void initialize_game(GameState *game, Difficulty difficulty);
void update_direction(GameState *game, Key key);
void update_game(GameState *game);

#endif
