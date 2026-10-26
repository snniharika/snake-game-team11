#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define WIDTH 20
#define HEIGHT 10
#define MAX_SCORE 1000000
#define ADMIN_PASSWORD "ChangeThisPassword"

typedef struct {
    int width;
    int height;
    int maxScore;
} GameConfig;

static int snakeX;
static int snakeY;
static int foodX;
static int foodY;
static int score = 0;
static int gameOver = 0;
static char direction = ' ';

static const GameConfig config = {
    WIDTH,
    HEIGHT,
    MAX_SCORE
};

/* Validate configuration once before the game starts */
static int isValidConfig(const GameConfig *cfg)
{
    return cfg != NULL &&
           cfg->width >= 5 && cfg->width <= 100 &&
           cfg->height >= 5 && cfg->height <= 50 &&
           cfg->maxScore >= 1 && cfg->maxScore <= MAX_SCORE;
}

/* Validate keyboard commands */
static int isValidDirection(char input)
{
    input = (char)tolower((unsigned char)input);

    return input == 'w' || input == 'a' ||
           input == 's' || input == 'd' ||
           input == 'm' || input == 'q';
}

/* Prevent immediate reverse movement */
static int isOppositeDirection(char current, char next)
{
    return (current == 'w' && next == 's') ||
           (current == 's' && next == 'w') ||
           (current == 'a' && next == 'd') ||
           (current == 'd' && next == 'a');
}

/* Keep score updates in one function */
static void awardFoodPoint(void)
{
    if (score < config.maxScore) {
        score++;
    }
}

/* Admin authentication */
static int isAdminAuthorized(void)
{
    char password[50];

    printf("Enter admin password: ");

    if (scanf("%49s", password) != 1) {
        return 0;
    }

    return strcmp(password, ADMIN_PASSWORD) == 0;
}

/* Admin operations */
static void adminMenu(void)
{
    char choice;

    if (!isAdminAuthorized()) {
        printf("Access denied.\n");
        return;
    }

    printf("\n=== Admin Menu ===\n");
    printf("1. View score\n");
    printf("2. Reset score\n");
    printf("3. Return to game\n");
    printf("Choose an option: ");

    if (scanf(" %c", &choice) != 1) {
        printf("Could not read admin choice.\n");
        return;
    }

    switch (choice) {
        case '1':
            printf("Current score: %d\n", score);
            break;
        case '2':
            score = 0;
            printf("Score reset.\n");
            break;
        case '3':
            printf("Returning to game.\n");
            break;
        default:
            printf("Invalid admin option.\n");
    }
}

/* Place food away from the snake */
static void placeFood(void)
{
    do {
        foodX = rand() % config.width;
        foodY = rand() % config.height;
    } while (foodX == snakeX && foodY == snakeY);
}

/* Draw the board */
static void drawBoard(void)
{
    int x, y;

    printf("\nScore: %d\n", score);

    for (y = 0; y < config.height; y++) {
        for (x = 0; x < config.width; x++) {
            if (x == snakeX && y == snakeY) {
                printf("O ");
            } else if (x == foodX && y == foodY) {
                printf("* ");
            } else {
                printf(". ");
            }
        }
        putchar('\n');
    }

    puts("W=up A=left S=down D=right M=admin Q=quit");
}

/* Read and normalize one command */
static int readCommand(char *input)
{
    if (scanf(" %c", input) != 1) {
        return 0;
    }

    *input = (char)tolower((unsigned char)*input);
    return 1;
}

int main(void)
{
    char input;

    if (!isValidConfig(&config)) {
        fprintf(stderr, "Invalid game configuration.\n");
        return EXIT_FAILURE;
    }

    snakeX = config.width / 2;
    snakeY = config.height / 2;

    srand((unsigned int)time(NULL));
    placeFood();

    puts("=== Console Snake Game ===");
    puts("Collect food (*) to earn points.");

    while (!gameOver) {
        drawBoard();
        printf("Enter command: ");

        if (!readCommand(&input)) {
            puts("\nInput ended. Exiting safely.");
            break;
        }

        if (!isValidDirection(input)) {
            puts("Invalid input. Use W, A, S, D, M or Q.");
            continue;
        }

        if (input == 'q') {
            puts("Game ended by player.");
            break;
        }

        if (input == 'm') {
            adminMenu();
            continue;
        }

        if (direction != ' ' &&
            isOppositeDirection(direction, input)) {
            puts("Immediate reversal is not allowed.");
            continue;
        }

        int nextX = snakeX;
        int nextY = snakeY;

        if (input == 'w') nextY--;
        else if (input == 's') nextY++;
        else if (input == 'a') nextX--;
        else if (input == 'd') nextX++;

        if (nextX < 0 || nextX >= config.width ||
            nextY < 0 || nextY >= config.height) {
            puts("Game Over! You hit a wall.");
            gameOver = 1;
            continue;
        }

        direction = input;
        snakeX = nextX;
        snakeY = nextY;

        if (snakeX == foodX && snakeY == foodY) {
            awardFoodPoint();
            puts("Food collected!");
            placeFood();
        }
    }

    printf("\nFinal score: %d\n", score);
    puts("Thanks for playing!");

    return EXIT_SUCCESS;
}
