#include "snake.h"
#include <conio.h>
#include <windows.h>

void initialize_input(void) {
    // No special initialization is needed for conio.h.
}

void shutdown_input(void) {
    // No special cleanup is needed for conio.h.
}

Key read_key(void) {
    if (!_kbhit())
        return KEY_NONE;

    int ch = _getch();

    // Windows arrow keys arrive as a two-part key code.
    if (ch == 0 || ch == 224) {
        ch = _getch();

        switch (ch) {
            case 72: return KEY_UP;
            case 80: return KEY_DOWN;
            case 75: return KEY_LEFT;
            case 77: return KEY_RIGHT;
            default: return KEY_NONE;
        }
    }

    switch (ch) {
        case 13: return KEY_ENTER;
        case 27: return KEY_ESCAPE;
        case 'w': case 'W': return KEY_UP;
        case 's': case 'S': return KEY_DOWN;
        case 'a': case 'A': return KEY_LEFT;
        case 'd': case 'D': return KEY_RIGHT;
        case 'x': case 'X': return KEY_QUIT;
        default: return KEY_NONE;
    }
}
