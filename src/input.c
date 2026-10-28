#include "snake.h"
#include <conio.h>

void initialize_input(void) {
    /* No terminal setup needed on Windows. */
}

void shutdown_input(void) {
    /* No terminal restoration needed on Windows. */
}

Key read_key(void) {
    if (!_kbhit()) {
        return KEY_NONE;
    }

    int ch = _getch();

    /* Handle arrow keys. */
    if (ch == 0 || ch == 224) {
        switch (_getch()) {
            case 72: return KEY_UP;
            case 80: return KEY_DOWN;
            case 75: return KEY_LEFT;
            case 77: return KEY_RIGHT;
            default: return KEY_NONE;
        }
    }

    switch (ch) {
        case 'w': case 'W': return KEY_UP;
        case 's': case 'S': return KEY_DOWN;
        case 'a': case 'A': return KEY_LEFT;
        case 'd': case 'D': return KEY_RIGHT;
        case '\r': return KEY_ENTER;
        case 27: return KEY_ESCAPE;
        case 'x': case 'X': return KEY_QUIT;
        case 'p': case 'P': return KEY_PAUSE;
        default: return KEY_NONE;
    }
}
