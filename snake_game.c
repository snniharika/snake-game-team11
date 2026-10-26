#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define WIDTH 20
#define HEIGHT 10
#define MAX_SCORE 1000000

/* Change this password before using the program */
#define ADMIN_PASSWORD "ChangeThisPassword"

static int snakeX = WIDTH / 2;
static int snakeY = HEIGHT / 2;
static int foodX;
static int foodY;
static int score = 0;
static int gameOver = 0;
static char direction = ' ';

/* Validate game input */
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

/* Only award points through this function */
static void awardFoodPoint(void)
{
    if (score < MAX_SCORE) {
        score++;
    }
}

/* Authenticate an administrator */
static int isAdminAuthorized(void)
{
    char password[50];

    printf("Enter admin password: ");

    if (scanf("%49s", password) != 1) {
        return 0;
    }

    if (strcmp(password, ADMIN_PASSWORD) == 0) {
        return 1;
    }

    return 0;
}

/* Admin menu */
static void adminMenu(void)
{
    char choice;

    if (!isAdminAuthorized()) {
        printf("Access denied. Incorrect password.\n");
        return;
    }

    printf("\n=== Admin Menu ===\n");
    printf("1. View current score\n");
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
            printf("Score reset successfully.\n");
            break;

        case '3':
            printf("Returning to game.\n");
            break;

        default:
            printf("Invalid admin option.\n");
    }
}

/* Place food away from the snake's current position */
static void placeFood(void)
{
    do {
        foodX = rand() % WIDTH;
        foodY = rand() % HEIGHT;
    } while (foodX == snakeX && foodY == snakeY);
}

/* Display the board */
static void drawBoard(void)
{
    int x, y;

    printf("\nScore: %d\n", score);

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
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

    printf("\nW=up A=left S=down D=right\n");
    printf("M=admin menu Q=quit\n");
}

int main(void)
{
    char input;

    srand((unsigned int)time(NULL));
    placeFood();

    printf("=== Console Snake Game ===\n");
    printf("Collect food (*) to earn points.\n");

    while (!gameOver) {
        drawBoard();
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

        direction = input;

        int nextX = snakeX;
        int nextY = snakeY;

        if (direction == 'w') {
            nextY--;
        } else if (direction == 's') {
            nextY++;
        } else if (direction == 'a') {
            nextX--;
        } else if (direction == 'd') {
            nextX++;
        }

        if (nextX < 0 || nextX >= WIDTH ||
            nextY < 0 || nextY >= HEIGHT) {
            printf("Game Over! You hit a wall.\n");
            gameOver = 1;
            continue;
        }

        snakeX = nextX;
        snakeY = nextY;

        if (snakeX == foodX && snakeY == foodY) {
            awardFoodPoint();
            printf("Food collected!\n");
            placeFood();
        }
    }

    printf("\nFinal score: %d\n", score);
    printf("Thanks for playing!\n");

    return 0;
}
