```c
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

/* Validate configuration */
static int isValidConfig(const GameConfig *config)
{
    return config != NULL &&
           config->width >= 5 && config->width <= 100 &&
           config->height >= 5 && config->height <= 50 &&
           config->maxScore >= 1 &&
           config->maxScore <= MAX_SCORE;
}

/* Validate commands */
static int isValidDirection(char input)
{
    input = (char)tolower((unsigned char)input);

    return input == 'w' || input == 'a' ||
           input == 's' || input == 'd' ||
           input == 'm' || input == 'q';
}

/* Prevent instant reverse movement */
static int isOppositeDirection(char current, char next)
{
    return (current == 'w' && next == 's') ||
           (current == 's' && next == 'w') ||
           (current == 'a' && next == 'd') ||
           (current == 'd' && next == 'a');
}

/* Award score only within the configured limit */
static void awardFoodPoint(const GameConfig *config)
{
    if (score < config->maxScore) {
        score++;
    }
}

/* Authenticate admin */
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
static void placeFood(const GameConfig *config)
{
    do {
        foodX = rand() % config->width;
        foodY = rand() % config->height;
    } while (foodX == snakeX && foodY == snakeY);
}

/* Draw board */
static void drawBoard(const GameConfig *config)
{
    int x, y;

    printf("\nScore: %d\n", score);

    for (y = 0; y < config->height; y++) {
        for (x = 0; x < config->width; x++) {
            if (x == snakeX && y == snakeY)
                printf("O ");
            else if (x == foodX && y == foodY)
                printf("* ");
            else
                printf(". ");
        }
        printf("\n");
    }

    printf("W=up A=left S=down D=right M=admin Q=quit\n");
}

/* Read one command; report end-of-input safely */
static int readCommand(char *input)
{
    int result = scanf(" %c", input);

    if (result != 1) {
        return 0;
    }

    *input = (char)tolower((unsigned char)*input);
    return 1;
}

int main(void)
{
    char input;

    const GameConfig config = {
        WIDTH,
        HEIGHT,
        MAX_SCORE
    };

    if (!isValidConfig(&config)) {
        fprintf(stderr, "Invalid game configuration.\n");
        return EXIT_FAILURE;
    }

    snakeX = config.width / 2;
    snakeY = config.height / 2;

    srand((unsigned int)time(NULL));
    placeFood(&config);

    printf("=== Console Snake Game ===\n");
    printf("Collect food (*) to earn points.\n");

    while (!gameOver) {
        drawBoard(&config);
        printf("Enter command: ");

        if (!readCommand(&input)) {
            printf("\nInput ended or could not be read. Exiting safely.\n");
            break;
        }

        if (!isValidDirection(input)) {
            printf("Invalid input. Use W, A, S, D, M or Q.\n");
            continue;
        }

        if (input == 'q') {
            printf("Game ended by player.\n");
            break;
        }

        if (input == 'm') {
            adminMenu();
            continue;
        }

        if (direction != ' ' &&
            isOppositeDirection(direction, input)) {
            printf("Immediate reversal is not allowed.\n");
            continue;
        }

        int nextX = snakeX;
        int nextY = snakeY;

        if (input == 'w') nextY--;
        else if (input == 's') nextY++;
        else if (input == 'a') nextX--;
        else if (input == 'd') nextX++;

        /* Check bounds before changing position */
        if (nextX < 0 || nextX >= config.width ||
            nextY < 0 || nextY >= config.height) {
            printf("Game Over! You hit a wall.\n");
            gameOver = 1;
            continue;
        }

        direction = input;
        snakeX = nextX;
        snakeY = nextY;

        if (snakeX == foodX && snakeY == foodY) {
            awardFoodPoint(&config);
            printf("Food collected!\n");
            placeFood(&config);
        }
    }

    printf("\nFinal score: %d\n", score);
    printf("Thanks for playing!\n");

    return EXIT_SUCCESS;
}
```
