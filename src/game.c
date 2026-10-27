#include "snake.h"
#include <stdio.h>
#include <windows.h>

static void clear_screen(void) {
    printf("\033[2J\033[H");
}

/* SNK-NF-006: Make the selected menu option easy to identify. */
void render_menu(int selected, Difficulty difficulty) {
    const char *levels[] = {"Easy", "Medium", "Hard"};

    clear_screen();

    printf("====================================\n");
    printf("             SNAKE GAME\n");
    printf("====================================\n\n");

    printf("MAIN MENU\n");
    printf("------------------------------------\n");
    printf("%s Start Game\n", selected == 0 ? ">>" : "  ");
    printf("%s Difficulty: %s\n",
           selected == 1 ? ">>" : "  ", levels[difficulty]);
    printf("%s Exit\n", selected == 2 ? ">>" : "  ");
    printf("------------------------------------\n");

    printf("\nCONTROLS\n");
    printf("  Up / Down arrows : Select menu option\n");
    printf("  Enter            : Confirm selection\n");
    printf("  W / A / S / D    : Move or navigate\n");
    printf("  X                : Exit game\n");

    printf("\nSTATUS\n");
    printf("  Current difficulty: %s\n", levels[difficulty]);
    printf("  Selected option is marked with >>\n");

    printf("\nChoose an option using Up/Down and press Enter.\n");
    fflush(stdout);
}

/* SNK-NF-006: Separate game status from the playing field. */
void render_game(const GameState *game) {
    if (game == NULL) {
        clear_screen();
        printf("ERROR: Game status is unavailable.\n");
        return;
    }

    clear_screen();

    printf("====================================\n");
    printf("             SNAKE GAME\n");
    printf("====================================\n");

    printf("GAME STATUS\n");
    printf("------------------------------------\n");
    printf("  Score      : %d\n", game->score);
    printf("  Length     : %d\n", game->length);
    printf("  Difficulty : %s\n",
           game->difficulty == EASY ? "Easy" :
           game->difficulty == MEDIUM ? "Medium" :
           game->difficulty == HARD ? "Hard" : "Unknown");
    printf("  State      : %s\n",
           game->game_over ? "GAME OVER" : "PLAYING");
    printf("------------------------------------\n\n");

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

    printf("\nSYMBOLS: O = Head | o = Body | * = Food | # = Wall\n");

    printf("\nCONTROLS\n");
    printf("  Arrow keys / WASD : Move\n");
    printf("  ESC               : Return to menu\n");
    printf("  X                 : Exit game\n");

    fflush(stdout);
}
