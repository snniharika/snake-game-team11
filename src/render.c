#include "snake.h"
#include <stdio.h>
#include <windows.h>

static void clear_screen(void) {
    /* Clear the screen and the scrollback, then move the cursor home. */
    printf("\033[2J\033[3J\033[H");
}

static const char *difficulty_name(Difficulty difficulty) {
    return difficulty == EASY ? "Easy" :
           difficulty == MEDIUM ? "Medium" : "Hard";
}

void render_menu(int selected, Difficulty difficulty) {
    const char *levels[] = {"Easy", "Medium", "Hard"};

    clear_screen();

    printf("          SNAKE GAME\n\n");

    printf("%s Start Game\n",
           selected == 0 ? "> " : "  ");

    printf("%s Difficulty: %s\n",
           selected == 1 ? "> " : "  ",
           levels[difficulty]);

    printf("%s Administrator Login\n",
           selected == 2 ? "> " : "  ");

    printf("%s Exit\n",
           selected == 3 ? "> " : "  ");

    printf("\nUse Up/Down and Enter/Space to select.\n");
    printf("WASD also works. X exits.\n");

    fflush(stdout);
}

static void draw_board(const GameState *game) {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            Position cell = {x, y};

            if (x == 0 || x == BOARD_WIDTH - 1 ||
                y == 0 || y == BOARD_HEIGHT - 1) {
                putchar('#');
            } else if (cell.x == game->segments[0].x &&
                       cell.y == game->segments[0].y) {
                putchar('O');
            } else if (cell.x == game->food.x &&
                       cell.y == game->food.y) {
                putchar('*');
            } else {
                int is_body = 0;

                for (int i = 1; i < game->length; i++) {
                    if (cell.x == game->segments[i].x &&
                        cell.y == game->segments[i].y) {
                        is_body = 1;
                        break;
                    }
                }

                putchar(is_body ? 'o' : ' ');
            }
        }
        putchar('\n');
    }
}

void render_game(const GameState *game) {
    clear_screen();

    printf("Score: %d | Difficulty: %s\n",
           game->score, difficulty_name(game->difficulty));

    draw_board(game);

    printf("\nArrow keys/WASD: Move | ESC/P: Pause | X: Exit\n");
    fflush(stdout);
}

/* SNK-F-013: Pause screen. The board is frozen exactly as it was. */
void render_pause_menu(const GameState *game, int selected) {
    clear_screen();

    printf("Score: %d | Difficulty: %s | *** PAUSED ***\n",
           game->score, difficulty_name(game->difficulty));

    draw_board(game);

    printf("\n%s Resume\n", selected == 0 ? ">" : " ");
    printf("%s Main Menu\n", selected == 1 ? ">" : " ");

    printf("\nUp/Down + Enter to select. ESC/P resumes. X exits.\n");
    fflush(stdout);
}

/* SNK-F-010 / SNK-F-012: Game Over screen with the final score and the
 * selectable Restart and Exit options. */
void render_game_over(const GameState *game, int selected) {
    clear_screen();

    printf("GAME OVER\n");
    printf("Final Score: %d | Difficulty: %s\n",
           game->score, difficulty_name(game->difficulty));

    draw_board(game);

    printf("\n%s Restart\n", selected == 0 ? ">" : " ");
    printf("%s Exit\n", selected == 1 ? ">" : " ");

    printf("\nUp/Down + Enter to select. ESC returns to the menu.\n");
    fflush(stdout);
}
