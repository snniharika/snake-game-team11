#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define WIDTH 20
#define HEIGHT 10

int snakeX = WIDTH / 2;
int snakeY = HEIGHT / 2;
int foodX = 5;
int foodY = 5;
int score = 0;

char direction = ' ';
int gameOver = 0;

/* Validate direction input */
int isValidDirection(char input) {
    input = (char)tolower((unsigned char)input);

    return input == 'w' || input == 'a' ||
           input == 's' || input == 'd' ||
           input == 'q';
}

/* Prevent an immediate reverse in direction */
int isOppositeDirection(char current, char next) {
    return (current == 'w' && next == 's') ||
           (current == 's' && next == 'w') ||
           (current == 'a' && next == 'd') ||
           (current == 'd' && next == 'a');
}

void drawBoard(void) {
    int x, y;

    printf("\nScore: %d\n", score);

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (x == snakeX && y == snakeY)
                printf("O");
            else if (x == foodX && y == foodY)
                printf("*");
            else
                printf(".");
        }
        printf("\n");
    }

    printf("Move: W=up A=left S=down D=right, Q=quit\n");
}

int main(void) {
    char input;

    printf("=== Snake Game ===\n");

    while (!gameOver) {
        drawBoard();
        printf("Enter direction: ");

        if (scanf(" %c", &input) != 1) {
            printf("Input error. Exiting game.\n");
            break;
        }

        input = (char)tolower((unsigned char)input);

        if (!isValidDirection(input)) {
            printf("Invalid input! Use W, A, S, D or Q.\n");
            continue;
        }

        if (input == 'q') {
            printf("Game ended. Final score: %d\n", score);
            break;
        }

        if (direction != ' ' &&
            isOppositeDirection(direction, input)) {
            printf("Invalid move! You cannot reverse direction.\n");
            continue;
        }

        direction = input;

        if (direction == 'w') snakeY--;
        if (direction == 's') snakeY++;
        if (direction == 'a') snakeX--;
        if (direction == 'd') snakeX++;

        if (snakeX < 0 || snakeX >= WIDTH ||
            snakeY < 0 || snakeY >= HEIGHT) {
            printf("Game Over! Final score: %d\n", score);
            gameOver = 1;
        } else if (snakeX == foodX && snakeY == foodY) {
            score++;
            foodX = rand() % WIDTH;
            foodY = rand() % HEIGHT;
        }
    }

    return 0;
}
