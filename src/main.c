#include "snake.h"
#include <stdio.h>
#include <stdlib.h>
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

typedef enum { AFTER_RESUME, AFTER_MENU, AFTER_EXIT } PauseResult;

/* SNK-F-013: Freeze the game and let the player resume or leave.
 * The GameState is never modified while paused, so resuming continues from
 * exactly the same snake position, food and score. */
static PauseResult run_pause_menu(GameState *game) {
    int selected = 0;

    set_paused(game, true);
    render_pause_menu(game, selected);

    while (1) {
        Key key = read_key();

        if (key == KEY_UP || key == KEY_DOWN) {
            selected = 1 - selected;
            render_pause_menu(game, selected);
        } else if (key == KEY_ESCAPE || key == KEY_PAUSE ||
                   (key == KEY_ENTER && selected == 0)) {
            set_paused(game, false);
            render_game(game);
            return AFTER_RESUME;
        } else if (key == KEY_ENTER) {
            return AFTER_MENU;
        } else if (key == KEY_QUIT) {
            return AFTER_EXIT;
        }

        Sleep(30);
    }
}

typedef enum { OVER_RESTART, OVER_MENU, OVER_EXIT } GameOverResult;

/* SNK-F-010 / SNK-F-012: Show the final score with Restart and Exit. */
static GameOverResult run_game_over_menu(const GameState *game) {
    int selected = 0;   /* Restart is the highlighted default. */

    render_game_over(game, selected);

    while (1) {
        Key key = read_key();

        if (key == KEY_UP || key == KEY_DOWN) {
            selected = 1 - selected;
            render_game_over(game, selected);
        } else if (key == KEY_ENTER) {
            return selected == 0 ? OVER_RESTART : OVER_EXIT;
        } else if (key == KEY_ESCAPE) {
            return OVER_MENU;
        } else if (key == KEY_QUIT) {
            return OVER_EXIT;
        }

        Sleep(30);
    }
}

/* Returns true to go back to the main menu, false when the player exits. */
static bool play_game(Difficulty difficulty) {
    while (1) {
        GameState game;
        initialize_game(&game, difficulty);
        render_game(&game);

        while (!game.game_over) {
            Key key = read_key();

            if (key == KEY_QUIT)
                return false;

            if (key == KEY_ESCAPE || key == KEY_PAUSE) {
                PauseResult paused = run_pause_menu(&game);

                if (paused == AFTER_MENU)
                    return true;
                if (paused == AFTER_EXIT)
                    return false;
                continue;
            }

            update_direction(&game, key);
            update_game(&game);
            render_game(&game);

            Sleep(movement_delay(difficulty));
        }

        GameOverResult over = run_game_over_menu(&game);

        if (over == OVER_MENU)
            return true;
        if (over == OVER_EXIT)
            return false;
        /* OVER_RESTART: loop around with a fresh game. */
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    initialize_input();

    int selected = 0;
    Difficulty difficulty = EASY;
    bool running = true;

    bool redraw = true;

    while (running) {
        /* Only redraw when something changed, otherwise the terminal
         * fills up with repeated copies of the menu. */
        if (redraw) {
            render_menu(selected, difficulty);
            redraw = false;
        }

        Key key = read_key();

        if (key != KEY_NONE)
            redraw = true;

        if (key == KEY_UP) {
            selected = (selected + 2) % 3;
        } else if (key == KEY_DOWN) {
            selected = (selected + 1) % 3;
        } else if (key == KEY_QUIT) {
            running = false;
        } else if (key == KEY_ENTER) {
            if (selected == 0) {
                running = play_game(difficulty);
            } else if (selected == 1) {
                difficulty = (Difficulty)((difficulty + 1) % 3);
            } else {
                running = false;
            }
        }

        Sleep(40);
    }

    shutdown_input();
    return 0;
}
