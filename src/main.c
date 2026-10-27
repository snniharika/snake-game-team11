#include "snake.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>

static int movement_delay(Difficulty difficulty) {
    switch (difficulty) {
        case EASY:   return 220;
        case MEDIUM: return 150;
        case HARD:   return 90;
    }
    return 150;
}

/* Returns false when the player chooses to exit. */
static bool play_game(Difficulty difficulty) {
    GameState game;
    initialize_game(&game, difficulty);

    while (!game.game_over) {
        Key key = read_key();

        if (key == KEY_ESCAPE)
            return true;

        if (key == KEY_QUIT)
            return false;

        update_direction(&game, key);
        update_game(&game);
        render_game(&game);

        Sleep(movement_delay(difficulty));
    }

    printf("\nGAME OVER! Final score: %d\n", game.score);
    printf("Press Enter to return to menu, or X to exit.\n");

    while (1) {
        Key key = read_key();

        if (key == KEY_ENTER || key == KEY_ESCAPE)
            return true;

        if (key == KEY_QUIT)
            return false;

        Sleep(30);
    }
}

static void start_new_game(Difficulty difficulty) {
    GameState game;
    initialize_game(&game, difficulty);

    printf("\nNew game initialized!\n");
    printf("Snake length: %d\n", game.length);
    printf("Starting score: %d\n", game.score);
    printf("Starting direction: Right\n");
    printf("Board size: %d x %d\n",
           BOARD_WIDTH, BOARD_HEIGHT);
}


int main(void) {
    Difficulty difficulty = EASY;
    initialize_input();

    int selected = 0;
    bool running = true;

    while (running) {
        render_menu(selected, difficulty);

        Key key = read_key();

        if (key == KEY_UP) {
            selected = (selected + 2) % 3;
        } else if (key == KEY_DOWN) {
            selected = (selected + 1) % 3;
        } else if (key == KEY_ENTER) {
            if (selected == 0) {
                start_new_game(difficulty);
                Sleep(1500);
            } else if (selected == 1) {
                difficulty = (Difficulty)((difficulty + 1) % 3);
            } else if (selected == 2) {
                running = false;
            }
        } else if (key == KEY_QUIT) {
            running = false;
        }

        Sleep(40);
    }

    shutdown_input();
    return 0;
}

