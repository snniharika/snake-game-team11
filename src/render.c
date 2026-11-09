
#include "snake.h"
#include <stdio.h>
#include <string.h>

#define ROW_EXTRA 4

/*
 * Validate the game state before accessing the segments array.
 * Use sizeof to obtain the actual array capacity.
 */
static bool valid_render_state(const GameState *game)
{
    if (game == NULL) {
        return false;
    }

    size_t segments_capacity =
        sizeof(game->segments) / sizeof(game->segments[0]);

    if (game->length < 1 ||
        (size_t)game->length > segments_capacity) {
        return false;
    }

    return true;
}

static void clear_screen(void)
{
    printf("\033[?25h");
    printf("\033[2J\033[3J\033[H");
}

static const char *difficulty_name(Difficulty difficulty)
{
    return difficulty == EASY ? "Easy" :
           difficulty == MEDIUM ? "Medium" : "Hard";
}

/*
 * Build the complete board in memory.
 * Validate the game state before accessing any snake segment.
 */
static bool build_board_frame(
    const GameState *game,
    char frame[BOARD_HEIGHT * (BOARD_WIDTH + ROW_EXTRA)])
{
    if (!valid_render_state(game) || frame == NULL) {
        return false;
    }

    char board[BOARD_HEIGHT][BOARD_WIDTH];

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (x == 0 || x == BOARD_WIDTH - 1 ||
                y == 0 || y == BOARD_HEIGHT - 1) {
                board[y][x] = '#';
            } else {
                board[y][x] = ' ';
            }
        }
    }

    /* Draw the snake body. */
    for (int i = 1; i < game->length; i++) {
        Position segment = game->segments[i];

        if (segment.x > 0 && segment.x < BOARD_WIDTH - 1 &&
            segment.y > 0 && segment.y < BOARD_HEIGHT - 1) {
            board[segment.y][segment.x] = 'o';
        }
    }

    /* Draw food only when it is inside the board. */
    if (game->food.x > 0 && game->food.x < BOARD_WIDTH - 1 &&
        game->food.y > 0 && game->food.y < BOARD_HEIGHT - 1) {
        board[game->food.y][game->food.x] = '*';
    }

    /* Draw the snake head last. */
    Position head = game->segments[0];

    if (head.x > 0 && head.x < BOARD_WIDTH - 1 &&
        head.y > 0 && head.y < BOARD_HEIGHT - 1) {
        board[head.y][head.x] = 'O';
    }

    /* Copy all rows into one output buffer. */
    size_t offset = 0;

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        memcpy(frame + offset, board[y], BOARD_WIDTH);
        offset += BOARD_WIDTH;

        memcpy(frame + offset, "\033[K\n", ROW_EXTRA);
        offset += ROW_EXTRA;
    }

    return true;
}

static void draw_board(const GameState *game)
{
    if (!valid_render_state(game)) {
        return;
    }

    char frame[BOARD_HEIGHT * (BOARD_WIDTH + ROW_EXTRA)];

    if (!build_board_frame(game, frame)) {
        return;
    }

    fwrite(frame, 1, sizeof(frame), stdout);
}

void render_menu(int selected, Difficulty difficulty)
{
    const char *levels[] = {"Easy", "Medium", "Hard"};

    if (difficulty < EASY || difficulty > HARD) {
        difficulty = EASY;
    }

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

void render_game(const GameState *game)
{
    if (!valid_render_state(game)) {
        return;
    }

    char output[4096];
    char board_frame[BOARD_HEIGHT * (BOARD_WIDTH + ROW_EXTRA)];

    if (!build_board_frame(game, board_frame)) {
        return;
    }

    int header_length = snprintf(
        output,
        sizeof(output),
        "\033[?25l\033[H"
        "Score: %-6d | Difficulty: %-8s\033[K\n",
        game->score,
        difficulty_name(game->difficulty));

    if (header_length < 0 ||
        (size_t)header_length + sizeof(board_frame) >= sizeof(output)) {
        return;
    }

    size_t used = (size_t)header_length;

    memcpy(output + used, board_frame, sizeof(board_frame));
    used += sizeof(board_frame);

    int footer_length = snprintf(
        output + used,
        sizeof(output) - used,
        "\nArrow keys/WASD: Move | ESC/P: Pause | X: Exit\n");

    if (footer_length < 0 ||
        (size_t)footer_length >= sizeof(output) - used) {
        return;
    }

    used += (size_t)footer_length;

    fwrite(output, 1, used, stdout);
    fflush(stdout);
}

/* SNK-F-013: Pause screen. */
void render_pause_menu(const GameState *game, int selected)
{
    if (!valid_render_state(game)) {
        return;
    }

    clear_screen();

    printf("Score: %d | Difficulty: %s | *** PAUSED ***\n",
           game->score, difficulty_name(game->difficulty));

    draw_board(game);

    printf("\n%s Resume\n", selected == 0 ? ">" : " ");
    printf("%s Main Menu\n", selected == 1 ? ">" : " ");

    printf("\nUp/Down + Enter to select. ESC/P resumes. X exits.\n");
    fflush(stdout);
}

/* SNK-F-010 / SNK-F-012: Game Over screen. */
void render_game_over(const GameState *game, int selected)
{
    if (!valid_render_state(game)) {
        return;
    }

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
