cp src/input.c src/input.c.windows-backup

cat > src/input.c <<'EOF'
#include "snake.h"
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <stdbool.h>

static struct termios original_terminal;
static bool terminal_initialized = false;

void initialize_input(void) {
    struct termios raw;

    if (tcgetattr(STDIN_FILENO, &original_terminal) == -1) {
        perror("tcgetattr");
        return;
    }

    raw = original_terminal;
    raw.c_lflag &= (tcflag_t) ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1) {
        perror("tcsetattr");
        return;
    }

    terminal_initialized = true;
}

void shutdown_input(void) {
    if (terminal_initialized) {
        tcsetattr(STDIN_FILENO, TCSANOW, &original_terminal);
        terminal_initialized = false;
    }
}

Key read_key(void) {
    fd_set readfds;
    struct timeval timeout = {0, 0};

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout) <= 0) {
        return KEY_NONE;
    }

    unsigned char ch;
    if (read(STDIN_FILENO, &ch, 1) != 1) {
        return KEY_NONE;
    }

    if (ch == '\033') {
        /* Read the rest of an arrow-key escape sequence if available. */
        struct timeval short_timeout = {0, 20000};
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        if (select(STDIN_FILENO + 1, &readfds, NULL, NULL,
                   &short_timeout) > 0) {
            unsigned char next;
            if (read(STDIN_FILENO, &next, 1) == 1 && next == '[') {
                if (read(STDIN_FILENO, &next, 1) == 1) {
                    switch (next) {
                        case 'A': return KEY_UP;
                        case 'B': return KEY_DOWN;
                        case 'D': return KEY_LEFT;
                        case 'C': return KEY_RIGHT;
                        default: return KEY_NONE;
                    }
                }
            }
        }

        return KEY_ESCAPE;
    }

    switch (ch) {
        case 'p': case 'P': return KEY_PAUSE;
        case '\n': case '\r': return KEY_ENTER;
        case 'w': case 'W': return KEY_UP;
        case 's': case 'S': return KEY_DOWN;
        case 'a': case 'A': return KEY_LEFT;
        case 'd': case 'D': return KEY_RIGHT;
        case 'x': case 'X': return KEY_QUIT;
        default: return KEY_NONE;
    }
}
EOF