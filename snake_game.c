#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define WIDTH 20
#define HEIGHT 10
#define MAX_SCORE 1000000

static int snakeX = WIDTH / 2;
static int snakeY = HEIGHT / 2;
static int foodX, foodY;
static int score = 0;
static int gameOver = 0;
static char direction = ' ';

/* Accept only supported commands */
static int isValidDirection(char input)
{
    input = (char)tolower((unsigned char)input);

    return input == 'w' || input == 'a' ||
           input == 's' || input == 'd' ||
           input == 'q';
}

/* Reject immediate reverse movement */
static int isOppositeDirection(char current, char next)
{
    return (current == 'w' && next == 's') ||
           (current == 's' && next == 'w') ||
           (current == 'a' && next == 'd') ||
           (current == 'd' && next == 'a');
}

/* The only function allowed to increase the score */
static void awardFoodPoint(void)
{
    if (score < MAX_SCORE) {
        score++;
    }
}

/* Place food somewhere other than the snake's position */
static void placeFood(void)
{
    do {
        foodX = rand() % WIDTH;
        foodY = rand() % HEIGHT;
    } while (foodX == snakeX && foodY == snakeY);
}

/* Draw the board */
static void drawBoard(void)
{
    int x, y;

    printf("\nScore: %d\n", score);

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (x == snakeX && y == snakeY)
                printf("O ");
            else if (x == foodX && y == foodY)
                printf("* ");
            else
                printf(". ");
        }
        printf("\n");
    }

    printf("W=up A=left S=down D=right Q=quit\n");
}

/* Read a single character */
static int readInput(char *input)
{
    if (scanf(" %c", input) != 1)
        return 0;

    *input = (char)tolower((unsigned char)*input);
    return 1;
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
        printf("Enter direction: ");

        if (!readInput(&input)) {
            printf("Input error. Exiting safely.\n");
            break;
        }

        if (!isValidDirection(input)) {
            printf("Invalid input. Use W, A, S, D or Q.\n");
            continue;
        }

        if (input == 'q') {
            printf("Game ended by player.\n");
            break;
        }

        if (direction != ' ' &&
            isOppositeDirection(direction, input)) {
            printf("Immediate reversal is not allowed.\n");
            continue;
        }

        direction = input;

        int nextX = snakeX;
        int nextY = snakeY;

        if (direction == 'w') nextY--;
        else if (direction == 's') nextY++;
        else if (direction == 'a') nextX--;
        else if (direction == 'd') nextX++;

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

    printf("Final score: %d\n", score);
    printf("Thanks for playing!\n");

    return 0;
}
