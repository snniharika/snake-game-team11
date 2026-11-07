#include "snake.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "admin.h"
#include <conio.h>
#include "persistence.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

static void handle_admin_login(void);

static int movement_delay(Difficulty difficulty) {
    switch (difficulty) {
        case EASY:   return 220;
        case MEDIUM: return 150;
        case HARD:   return 90;
    }
    return 150;
}

typedef enum {
    AFTER_RESUME,
    AFTER_MENU,
    AFTER_EXIT
} PauseResult;

typedef enum {
    OVER_RESTART,
    OVER_MENU,
    OVER_EXIT
} GameOverResult;

/* Convert the selected difficulty to the format stored on disk. */
static const char *difficulty_name(Difficulty difficulty) {
    switch (difficulty) {
        case EASY:
            return "Easy";
        case MEDIUM:
            return "Medium";
        case HARD:
            return "Hard";
        default:
            return "Easy";
    }
}

/* Persist statistics once a game has reached its game-over state. */
static bool save_completed_game(const GameState *game) {
    if (game == NULL || game->score < 0) {
        return false;
    }

    ProtectedGameData data;

    if (!persistence_load_game_data(&data)) {
        printf("\nWarning: Could not read saved game data. Statistics were not updated.\n");
        return false;
    }

    if (game->score > data.high_score) {
        data.high_score = game->score;
    }

    /* Keep the counter within the persistence module's accepted range. */
    if (data.games_played < 2147483647U) {
        data.games_played++;
    }

    data.last_score = game->score;
    strcpy(data.last_difficulty, difficulty_name(game->difficulty));

    if (!persistence_save_game_data(&data)) {
        printf("\nWarning: Could not save game statistics to data/game_data.txt.\n");
        return false;
    }

    return true;
}

/* Pause the game without modifying its current state. */
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
        } else if (key == KEY_ENTER && selected == 1) {
            return AFTER_MENU;
        } else if (key == KEY_QUIT) {
            return AFTER_EXIT;
        }

        Sleep(5);
    }
}

/* Display the final score and let the player choose what happens next. */
static GameOverResult run_game_over_menu(GameState *game) {
    int selected = 0;

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

        Sleep(5);
    }
}

/*
 * Returns true when the player chooses the main menu,
 * and false when the player chooses to exit.
 */
static bool play_game(Difficulty difficulty, FILE *timing_log) {
    while (1) {
        GameState game;
        initialize_game(&game, difficulty);
        render_game(&game);

        DWORD move_interval = (DWORD)movement_delay(difficulty);
        ULONGLONG last_tick = GetTickCount64();
        ULONGLONG next_tick = last_tick + move_interval;

        while (!game.game_over) {
            Key key = KEY_NONE;

            /*
             * Drain buffered input so the latest direction is handled
             * without waiting for the next movement tick.
             */
            while (1) {
                Key next_key = read_key();

                if (next_key == KEY_NONE) {
                    break;
                }

                if (next_key == KEY_ESCAPE ||
                    next_key == KEY_QUIT ||
                    next_key == KEY_PAUSE) {
                    key = next_key;
                    break;
                }

                key = next_key;
            }

            if (key == KEY_QUIT) {
                return false;
            }

            if (key == KEY_ESCAPE || key == KEY_PAUSE) {
                PauseResult paused = run_pause_menu(&game);

                if (paused == AFTER_MENU) {
                    return true;
                }

                if (paused == AFTER_EXIT) {
                    return false;
                }

                last_tick = GetTickCount64();
                next_tick = last_tick + move_interval;
                continue;
            }

            if (key != KEY_NONE) {
                update_direction(&game, key);
            }

            ULONGLONG now = GetTickCount64();

            /*
             * Run updates on a scheduled tick deadline. This avoids
             * accumulating timing drift from the work done each tick.
             */
            if (now >= next_tick) {
                DWORD actual_interval = (DWORD)(now - last_tick);

                update_game(&game);
                render_game(&game);

                /* Record tick timing for SNK-NF-003 performance testing. */
                if (timing_log != NULL) {
                    DWORD deviation = actual_interval > move_interval
                                          ? actual_interval - move_interval
                                          : move_interval - actual_interval;
                    double jitter_percent =
                        ((double)deviation / (double)move_interval) * 100.0;

                    fprintf(timing_log, "%lu,%lu,%.2f\n",
                            (unsigned long)move_interval,
                            (unsigned long)actual_interval,
                            jitter_percent);
                }

                last_tick = now;
                next_tick += move_interval;

                /* Skip missed deadlines rather than running catch-up ticks. */
                if (now >= next_tick) {
                    next_tick = now + move_interval;
                }
            }

            /* Poll input frequently without changing the game tick rate. */
            Sleep(1);
        }

        /* Save the completed game's score and statistics before showing the menu. */
        (void)save_completed_game(&game);

        GameOverResult over = run_game_over_menu(&game);

        if (over == OVER_MENU) {
            return true;
        }

        if (over == OVER_EXIT) {
            return false;
        }

        /* OVER_RESTART: begin a fresh game. */
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    initialize_input();

    /* Open a CSV log used to measure game-loop tick timing. */
    FILE *timing_log = fopen("game_loop_timing.csv", "a");

    if (timing_log != NULL) {
        fseek(timing_log, 0, SEEK_END);

        if (ftell(timing_log) == 0) {
            fprintf(timing_log,
                    "expected_interval_ms,actual_interval_ms,jitter_percent\n");
        }
    } else {
        printf("Warning: Unable to create game loop timing log.\n");
    }

    int selected = 0;
    Difficulty difficulty = EASY;
    bool running = true;
    bool menu_needs_redraw = true;

    while (running) {
        /*
         * Redraw only when the visible menu state changes.
         */
        if (menu_needs_redraw) {
            render_menu(selected, difficulty);
            menu_needs_redraw = false;
        }

        Key key = read_key();

        if (key == KEY_UP) {
            selected = (selected + 3) % 4;
            menu_needs_redraw = true;
        } else if (key == KEY_DOWN) {
            selected = (selected + 1) % 4;
            menu_needs_redraw = true;
        } else if (key == KEY_QUIT || key == KEY_ESCAPE) {
            running = false;
        } else if (key == KEY_ENTER) {
            if (selected == 0) {
                running = play_game(difficulty, timing_log);
                menu_needs_redraw = true;
            } else if (selected == 1) {
                difficulty = (Difficulty)((difficulty + 1) % 3);
                menu_needs_redraw = true;
            } else if (selected == 2) {
                handle_admin_login();
                menu_needs_redraw = true;
            } else {
                running = false;
            }
        }

        /*
         * Poll frequently without continuously redrawing
         * an unchanged menu.
         */
        Sleep(5);
    }

    printf("\033[?25h"); //cursor restored after the game
    shutdown_input();

    if (timing_log != NULL) {
        fclose(timing_log);
    }

    return 0;
}

/* Read an administrator password without displaying its characters. */
static bool read_admin_password(char *password, size_t capacity) {
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


/*
 * Read a non-negative integer from the administrator.
 */
static bool read_nonnegative_number(int *value)
{
    char line[64];
    char *end;

    if (value == NULL) {
        return false;
    }

    if (fgets(line, sizeof(line), stdin) == NULL) {
        return false;
    }

    errno = 0;

    long number = strtol(line, &end, 10);

    if (end == line || errno == ERANGE ||
        number < 0 || number > INT_MAX) {
        return false;
    }

    while (*end != '\0') {
        if (!isspace((unsigned char)*end)) {
            return false;
        }
        end++;
    }

    *value = (int)number;
    return true;
}

/*
 * Allow an authenticated Administrator to modify
 * one stored score or configuration value.
 */
static void handle_admin_data_update(void)
{
    if (!admin_is_authenticated()) {
        printf("\nAccess denied. Administrator authentication required.\n");
        return;
    }

    ProtectedGameData data;

    if (!persistence_load_game_data(&data)) {
        printf("\nUnable to load protected game data.\n");
        return;
    }

    printf("\n========== ADMINISTRATOR OPERATIONS ==========\n");
    printf("1. Modify highest score\n");
    printf("2. Modify total games played\n");
    printf("3. Modify last game score\n");
    printf("4. Modify last game difficulty\n");
    printf("5. Cancel\n");
    printf("6. Reset all stored game data\n");
    printf("Select an option: ");

    int choice;

    if (!read_nonnegative_number(&choice)) {
        printf("\nInvalid input. No changes were made.\n");
        return;
    }

    if (choice == 5) {
        printf("\nModification cancelled.\n");
        return;
    }

    switch (choice) {
        case 1: {
            int value;

            printf("Enter new highest score: ");

            if (!read_nonnegative_number(&value)) {
                printf("\nInvalid score. No changes were made.\n");
                return;
            }

            data.high_score = value;
            break;
        }

        case 2: {
            int value;

            printf("Enter new total games played: ");

            if (!read_nonnegative_number(&value)) {
                printf("\nInvalid value. No changes were made.\n");
                return;
            }

            data.games_played = (unsigned int)value;
            break;
        }

        case 3: {
            int value;

            printf("Enter new last game score: ");

            if (!read_nonnegative_number(&value)) {
                printf("\nInvalid score. No changes were made.\n");
                return;
            }

            data.last_score = value;
            break;
        }

        case 4: {
            char difficulty[32];

            printf("Enter difficulty (Easy/Medium/Hard): ");

            if (fgets(difficulty, sizeof(difficulty), stdin) == NULL) {
                printf("\nUnable to read difficulty. No changes were made.\n");
                return;
            }

            difficulty[strcspn(difficulty, "\r\n")] = '\0';

            if (strcmp(difficulty, "Easy") != 0 &&
                strcmp(difficulty, "Medium") != 0 &&
                strcmp(difficulty, "Hard") != 0) {
                printf("\nInvalid difficulty. No changes were made.\n");
                return;
            }

            strcpy(data.last_difficulty, difficulty);
            break;
        }

        case 6: {
    char confirmation;

    printf("\nWARNING: This will reset all stored game data.\n");
    printf("Are you sure? (Y/N): ");

    if (scanf(" %c", &confirmation) != 1) {
        printf("\nUnable to read confirmation. Reset cancelled.\n");
        return;
    }

    /*
     * Clear the remaining characters from the input line.
     */
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }

    if (confirmation != 'Y' && confirmation != 'y') {
        printf("\nReset cancelled. No data was changed.\n");
        return;
    }

    if (admin_reset_protected_data()) {
        printf("\nUpdated protected data:\n");
        admin_view_protected_data();
    }

    return;
}

        default:
            printf("\nInvalid option. No changes were made.\n");
            return;
    }

    /*
     * The admin module performs the authentication check
     * again before saving the modified values.
     */
    if (admin_save_protected_data(&data)) {
        printf("\nUpdated protected data:\n");
        admin_view_protected_data();
    }
}

static void handle_admin_login(void)
{
    char password[1025] = {0};

    if (!admin_is_authenticated()) {
        if (!read_admin_password(password, sizeof(password))) {
            printf("\nPassword input failed or was empty.\n");
            printf("Press any key to return to the menu...\n");
            (void)_getch();
            return;
        }

        bool success = admin_authenticate(password);

        SecureZeroMemory(password, sizeof(password));

        if (!success) {
            printf("\nAdministrator authentication failed.\n");
            printf("Protected data access denied.\n");
            printf("Press any key to return to the menu...\n");
            (void)_getch();
            return;
        }

        printf("\nAdministrator authentication successful.\n");
    }
    else {
        printf("\nAdministrator is already authenticated.\n");
    }

    admin_view_protected_data();

    printf("\n");
    handle_admin_data_update();

    printf("\nPress any key to return to the menu...\n");
    (void)_getch();
}
