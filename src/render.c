#include "snake.h"
#include <stdio.h>

static void clear_screen(void) {
    printf("\033[2J\033[H");
}



void render_menu(int selected, Difficulty difficulty) {
    const char *levels[] = {"Easy", "Medium", "Hard"};
    const char *options[] = {
        "Start Game",
        "Difficulty Select",
        "Exit"
    };

    printf("\033[2J\033[H");
    printf("          SNAKE GAME\n");

    for (int i = 0; i < 3; i++) {
        printf("%s ", selected == i ? ">" : " ");

        if (i == 1) {
            printf("%s: %s\n", options[i], levels[difficulty]);
        } else {
            printf("%s\n", options[i]);
        }
    }

    printf("\nUse UP/DOWN arrows and ENTER.\n");
    fflush(stdout);
}


void render_game(const GameState *game) {
    clear_screen();

    printf("Score: %d | Difficulty: %s\n",
           game->score,
           game->difficulty == EASY ? "Easy" :
           game->difficulty == MEDIUM ? "Medium" : "Hard");

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

    printf("\nArrow keys/WASD: Move | ESC: Menu | X: Exit\n");
    fflush(stdout);
}
