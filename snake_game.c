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

static const GameConfig config = {
    WIDTH, HEIGHT, MAX_SCORE
};

static int snakeX, snakeY;
static int foodX, foodY;
static int score = 0;
static int gameOver = 0;
static char direction = ' ';

static int isValidConfig(const GameConfig *cfg)
{
    return cfg != NULL &&
           cfg->width >= 5 && cfg->width <= 100 &&
           cfg->height >= 5 && cfg->height <= 50 &&
           cfg->maxScore >= 1 &&
           cfg->maxScore <= MAX_SCORE;
}

static int isValidDirection(char input)
{
    input = (char)tolower((unsigned char)input);

    return input == 'w' || input == 'a' ||
           input == 's' || input == 'd' ||
           input == 'm' || input == 'q';
}

static int isOppositeDirection(char current, char next)
{
    return (current == 'w' && next == 's') ||
           (current == 's' && next == 'w') ||
           (current == 'a' && next == 'd') ||
           (current == 'd' && next == 'a');
}

static void awardFoodPoint(void)
{
    if (score < config.maxScore) {
        score++;
    }
}

static int isAdminAuthorized(void)
{
    char password[50];

    printf("Admin password: ");

    if (scanf("%49s", password) != 1) {
        return 0;
    }

    return strcmp(password, ADMIN_PASSWORD) == 0;
}

static void adminMenu(void)
{
    char choice;

    if (!isAdminAuthorized()) {
        puts("Access denied. Returning to the game.");
        return;
    }

    puts("\n--- ADMIN MENU ---");
    puts("1. View current score");
    puts("2. Reset score");
    puts("3. Return to game");
    printf("Choose 1, 2 or 3: ");

    if (scanf(" %c", &choice) != 1) {
        puts("Could not read your choice.");
        return;
    }

    switch (choice) {
        case '1':
            printf("Current score: %d\n", score);
            break;
        case '2':
            score = 0;
            puts("Score has been reset.");
            break;
        case '3':
            puts("Returning to the game...");
            break;
        default:
            puts("Invalid choice. Please select 1, 2 or 3.");
    }
}

static void placeFood(void)
{
    do {
        foodX = rand() % config.width;
        foodY = rand() % config.height;
    } while (foodX == snakeX && foodY == snakeY);
}

static void showInstructions(void)
{
    puts("\n================================");
    puts("       WELCOME TO SNAKE");
    puts("================================");
    puts("GOAL: Move the snake onto * to earn points.");
    puts("");
    puts("CONTROLS:");
    puts("  W - Move up");
    puts("  A - Move left");
    puts("  S - Move down");
    puts("  D - Move right");
    puts("  M - Open admin menu");
    puts("  Q - Quit the game");
    puts("");
    puts("RULES:");
    puts("  - Do not hit the walls.");
    puts("  - Immediate reverse moves are not allowed.");
    puts("  - Enter one command at a time.");
    puts("================================\n");
}

static void drawBoard(void)
{
    int x, y;

    printf("\nYour score: %d\n\n", score);

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

    puts("\nSnake: O    Food: *    Empty space: .");
}

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
        fprintf(stderr, "Error: Invalid game settings.\n");
        return EXIT_FAILURE;
    }

    snakeX = config.width / 2;
    snakeY = config.height / 2;

    srand((unsigned int)time(NULL));
    placeFood();
    showInstructions();

    puts("The game is ready. Good luck!");

    while (!gameOver) {
        drawBoard();
        printf("Enter W, A, S, D, M or Q: ");

        if (!readCommand(&input)) {
            puts("\nInput could not be read. The game will close safely.");
            break;
        }

        if (!isValidDirection(input)) {
            puts("Sorry, that command is not recognised.");
            puts("Please enter W, A, S, D, M or Q.");
            continue;
        }

        if (input == 'q') {
            puts("You chose to quit.");
            break;
        }

        if (input == 'm') {
            adminMenu();
            continue;
        }

        if (direction != ' ' &&
            isOppositeDirection(direction, input)) {
            puts("That move reverses your direction.");
            puts("Choose another direction.");
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
            puts("\nGAME OVER: You hit a wall.");
            gameOver = 1;
            continue;
        }

        direction = input;
        snakeX = nextX;
        snakeY = nextY;

        if (snakeX == foodX && snakeY == foodY) {
            awardFoodPoint();
            puts("Nice! You collected food and earned a point.");
            printf("Your score is now %d.\n", score);
            placeFood();
        }
    }

    puts("\n================================");
    puts("           GAME SUMMARY");
    puts("================================");
    printf("Final score: %d\n", score);
    puts("Thanks for playing Snake!");
    puts("================================");

    return EXIT_SUCCESS;
}
