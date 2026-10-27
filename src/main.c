#include "snake.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "admin.h"
#include <conio.h>


static void handle_admin_login(void);

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

    /*
     * Render the initial board before the first movement.
     * Input polling and snake movement use separate timings.
     */
    render_game(&game);

    DWORD last_move = GetTickCount();
    DWORD move_interval = (DWORD)movement_delay(difficulty);

    while (!game.game_over) {
        /*
         * Poll keyboard input frequently instead of waiting
         * for the next movement interval to finish.
         */
        Key key = KEY_NONE;

/*
 * Drain buffered keyboard input so recent input is processed
 * without waiting for the next movement tick.
 */
while (1) {
            Key next_key = read_key();

            if (next_key == KEY_NONE) {
                break;
            }

            /*
             * Exit and menu commands should be handled immediately.
             */
            if (next_key == KEY_ESCAPE || next_key == KEY_QUIT) {
                key = next_key;
                break;
            }

            /*
             * Keep the most recently read direction.
             */
            key = next_key;
        }

        if (key == KEY_ESCAPE) {
            return true;
        }

        if (key == KEY_QUIT) {
            return false;
        }

        update_direction(&game, key);

        if (key == KEY_ESCAPE)
            return true;

        if (key == KEY_QUIT)
            return false;

        /*
         * Register the requested direction immediately.
         * update_direction() rejects illegal 180-degree turns.
         */
        update_direction(&game, key);

        /*
         * Move the snake only when its movement interval
         * has elapsed. Polling more frequently must not
         * increase the snake's movement speed.
         */
        DWORD now = GetTickCount();

        if ((DWORD)(now - last_move) >= move_interval) {
            update_game(&game);
            render_game(&game);

            last_move = now;
        }

        /*
         * Short sleep prevents unnecessary CPU usage while
         * keeping keyboard polling responsive.
         */
        Sleep(5);
    }

    printf("\nGAME OVER! Final score: %d\n", game.score);
    printf("Press Enter to return to menu, or X to exit.\n");

    while (1) {
        Key key = read_key();

        if (key == KEY_ENTER || key == KEY_ESCAPE)
            return true;

        if (key == KEY_QUIT)
            return false;

        Sleep(5);
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    initialize_input();


    int selected = 0;
    Difficulty difficulty = EASY;
    bool running = true;
    bool menu_needs_redraw = true;

    while (running) {
    /*
     * Redraw the menu only when its visible state changes.
     */
        if (menu_needs_redraw) {
            render_menu(selected, difficulty);
            menu_needs_redraw = false;
        }

        Key key = read_key();

        if (key == KEY_UP) {
            selected = (selected + 3) % 4;
            menu_needs_redraw = true;
        }
        else if (key == KEY_DOWN) {
            selected = (selected + 1) % 4;
            menu_needs_redraw = true;
        }
        else if (key == KEY_QUIT) {
            running = false;
        }
        else if (key == KEY_ENTER) {
            if (selected == 0) {
                running = play_game(difficulty);
                menu_needs_redraw = true;
            }
            else if (selected == 1) {
                difficulty = (Difficulty)((difficulty + 1) % 3);
                menu_needs_redraw = true;
            }
            else if (selected == 2) {
                handle_admin_login();
                menu_needs_redraw = true;
            }
            else {
                running = false;
            }
        }

    /*
     * Poll frequently without continuously redrawing
     * an unchanged menu.
     */
    Sleep(5);
}

    shutdown_input();
    return 0;
}



static bool read_admin_password(char *password, size_t capacity)
{
    if (password == NULL || capacity < 2) {
        return false;
    }

    size_t length = 0;
    int ch;

    HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
    DWORD original_mode;

    if (console == INVALID_HANDLE_VALUE ||
        !GetConsoleMode(console, &original_mode)) {
        return false;
    }

    DWORD hidden_mode =
        original_mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);

    if (!SetConsoleMode(console, hidden_mode)) {
        return false;
    }

    printf("Enter Administrator password: ");
    fflush(stdout);

    bool valid = true;

    while (1) {
        ch = _getch();

        if (ch == '\r' || ch == '\n') {
            break;
        }

        if (ch == 3) {
            valid = false;
            break;
        }

        if (ch == '\b') {
            if (length > 0) {
                length--;
            }
            continue;
        }

        if (ch == 0 || ch == 224) {
            (void)_getch();
            continue;
        }

        if (ch < 32 || ch > 126) {
            continue;
        }

        if (length >= capacity - 1) {
            valid = false;
            break;
        }

        password[length++] = (char)ch;
    }

    password[length] = '\0';

    if (!SetConsoleMode(console, original_mode)) {
        SecureZeroMemory(password, capacity);
        return false;
    }

    putchar('\n');

    if (!valid || length == 0) {
        SecureZeroMemory(password, capacity);
        return false;
    }

    return true;
}




static void handle_admin_login(void)
{
    char password[1025] = {0};

    if (admin_is_authenticated()) {
        printf("Administrator is already authenticated.\n");
        printf("Press any key to return to the menu...\n");
        (void)_getch();
        return;
    }

    if (!read_admin_password(password, sizeof(password))) {
        printf("Password input failed or was empty.\n");
        printf("Press any key to return to the menu...\n");
        (void)_getch();
        return;
    }

    bool success = admin_authenticate(password);

    SecureZeroMemory(password, sizeof(password));

    if (success) {
        printf("Administrator authentication successful.\n");
    } else {
        printf("Administrator authentication failed.\n");
    }

    printf("Press any key to return to the menu...\n");
    (void)_getch();
}
