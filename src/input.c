#include "snake.h"
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <ctype.h>

static struct termios original_terminal;
static int terminal_initialized = 0;

void initialize_input(void) {
    struct termios raw;

    if (tcgetattr(STDIN_FILENO, &original_terminal) == -1)
        return;

    raw = original_terminal;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0)
        terminal_initialized = 1;
}

void shutdown_input(void) {
    if (terminal_initialized) {
        tcsetattr(STDIN_FILENO, TCSANOW, &original_terminal);
        terminal_initialized = 0;
    }
}

static int key_available(int timeout_us) {
    fd_set set;
    struct timeval timeout = {0, timeout_us};

    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    return select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout) > 0;
}

Key read_key(void) {
    unsigned char ch;

    if (!key_available(0))
        return KEY_NONE;

    if (read(STDIN_FILENO, &ch, 1) != 1)
        return KEY_NONE;

    if (ch == 27) {
        if (!key_available(20000))
            return KEY_ESCAPE;

        unsigned char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1 ||
            seq[0] != '[')
            return KEY_ESCAPE;

        if (read(STDIN_FILENO, &seq[1], 1) != 1)
            return KEY_ESCAPE;

        switch (seq[1]) {
            case 'A': return KEY_UP;
            case 'B': return KEY_DOWN;
            case 'C': return KEY_RIGHT;
            case 'D': return KEY_LEFT;
            default: return KEY_NONE;
        }
    }

    switch (tolower(ch)) {
        case 'w': return KEY_UP;
        case 's': return KEY_DOWN;
        case 'a': return KEY_LEFT;
        case 'd': return KEY_RIGHT;
        case '\r':
        case '\n': return KEY_ENTER;
        case 'x': return KEY_QUIT;
        default: return KEY_NONE;
    }
}
