#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define WIDTH 20
#define HEIGHT 10
#define MAX_SCORE 1000000
#define ADMIN_PASSWORD "ChangeThisPassword"

static int snakeX = WIDTH / 2;
static int snakeY = HEIGHT / 2;
static int foodX, foodY;
static int score = 0;
static int gameOver = 0;
static char direction = ' ';

/* Configuration structure */
typedef struct {
    int width;
    int height;
    int maxScore;
} GameConfig;

/* Validate configuration before starting */
static int isValidConfig(const GameConfig *config)
{
    if (config == NULL) {
        return 0;
    }

    if (config->width < 5 || config->width > 100) {
        return 0;
    }

    if (config->height < 5 || config->height > 50) {
        return 0;
    }

    if (config->maxScore < 1 ||
        config->maxScore > MAX_SCORE) {
        return 0;
    }

    return 1;
}

/* Validate game commands */
static int isValidDirection(char input)
{
    input = (char)tolower((unsigned char)input);

    return input == 'w' || input == 'a' ||
           input == 's' || input == 'd' ||
           input == 'q' || input == 'm';
}

/* Prevent immediate reverse movement */
static int isOppositeDirection(char current, char next)
{
    return (current == 'w' && next == 's') ||
           (current == 's' && next == 'w') ||
           (current == 'a' && next == 'd') ||
           (current == 'd' && next == 'a');
}

/* Controlled score update */
static void awardFoodPoint(const GameConfig *config)
{
    if (score < config->maxScore) {
        score++;
    }
}

/* Check admin password */
static int isAdminAuthorized(void)
{
    char password[50];

    printf("Enter admin password: ");

    if (scanf("%49s", password) != 1) {
        return 0;
    }

    return strcmp(password, ADMIN_PASSWORD) == 0;
}

/* Admin menu */
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
        printf("Invalid input.\n");
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

/* Draw the game board */
static void drawBoard(const GameConfig *config)
{
    int x, y;

    printf("\nScore: %d\n", score);

    for (y = 0; y < config->height; y++) {
        for (x = 0; x < config->width; x++) {
            if (x == snakeX && y == snakeY) {
                printf("O ");
            } else if (x == foodX && y == foodY) {
                printf("* ");
            } else {
                printf(". ");
            }
        }
        printf("\n");
    }

    printf("W=up A=left S=down D=right M=admin Q=quit\n");
}

int main(void)
{
    char input;

    /* Default configuration */
    const GameConfig config = {
        WIDTH,
        HEIGHT,
        MAX_SCORE
    };

    /* Reject invalid configuration */
    if (!isValidConfig(&config)) {
        fprintf(stderr, "Invalid game configuration.\n");
        return EXIT_FAILURE;
    }

    srand((unsigned int)time(NULL));
    placeFood(&config);

    printf("=== Console Snake Game ===\n");
    printf("Collect food (*) to earn points.\n");

    while (!gameOver) {
        drawBoard(&config);
        printf("Enter command: ");

        if (scanf(" %c", &input) != 1) {
            printf("Input error. Exiting safely.\n");
            break;
        }

        input = (char)tolower((unsigned char)input);

        if (!isValidDirection(input)) {
            printf("Invalid input. Use W, A, S, D, M or Q.\n");
            continue;
        }

        if (input == 'q') {
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

    printf("Final score: %d\n", score);
    printf("Thanks for playing!\n");

    return EXIT_SUCCESS;
}
