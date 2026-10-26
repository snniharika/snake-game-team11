#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define WIDTH 20
#define HEIGHT 10

#define INITIAL_SCORE 0
#define MAX_SCORE 1000000

int snakeX = WIDTH / 2;
int snakeY = HEIGHT / 2;

int foodX;
int foodY;

int score = INITIAL_SCORE;
int gameOver = 0;

char direction = ' ';

/* Validate allowed keyboard input */
int isValidDirection(char input)
{
    input = (char)tolower((unsigned char)input);

    return input == 'w' ||
           input == 'a' ||
           input == 's' ||
           input == 'd' ||
           input == 'q';
}

/* Prevent immediate reversal */
int isOppositeDirection(char current, char next)
{
    return (current == 'w' && next == 's') ||
           (current == 's' && next == 'w') ||
           (current == 'a' && next == 'd') ||
           (current == 'd' && next == 'a');
}

/* Generate food away from the snake's current position */
void placeFood(void)
{
    do {
        foodX = rand() % WIDTH;
        foodY = rand() % HEIGHT;
    } while (foodX == snakeX && foodY == snakeY);
}

/* Display the game board */
void drawBoard(void)
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

    printf("\nControls: W=up, A=left, S=down, D=right, Q=quit\n");
}

/* Read one character safely */
int readInput(char *input)
{
    int result = scanf(" %c", input);

    if (result != 1) {
        return 0;
    }

    *input = (char)tolower((unsigned char)*input);
    return 1;
}

/* Main game */
int main(void)
{
    char input;

    srand((unsigned int)time(NULL));
    placeFood();

    printf("=== Console Snake Game ===\n");
    printf("Collect food (*) to increase your score.\n");

    while (!gameOver) {
        drawBoard();
        printf("Enter direction: ");

        if (!readInput(&input)) {
            printf("\nInput error. Game ended safely.\n");
            break;
        }

        /* Reject invalid input */
        if (!isValidDirection(input)) {
            printf("Invalid input! Enter W, A, S, D or Q.\n");
            continue;
        }

        /* Quit safely */
        if (input == 'q') {
            printf("Game ended. Final score: %d\n", score);
            break;
        }

        /* Reject an immediate reverse */
        if (direction != ' ' &&
            isOppositeDirection(direction, input)) {
            printf("Invalid move! Immediate reversal is not allowed.\n");
            continue;
        }

        direction = input;

        /* Calculate the next position */
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

        /* Check wall collision before updating position */
        if (nextX < 0 || nextX >= WIDTH ||
            nextY < 0 || nextY >= HEIGHT) {
            printf("Game Over! You hit a wall.\n");
            gameOver = 1;
            continue;
        }

        snakeX = nextX;
        snakeY = nextY;

        /* Collect food and protect against score overflow */
        if (snakeX == foodX && snakeY == foodY) {
            if (score < MAX_SCORE) {
                score++;
            }

            printf("Food collected! Score: %d\n", score);
            placeFood();
        }
    }

    printf("Final score: %d\n", score);
    printf("Thank you for playing!\n");

    return 0;
}
