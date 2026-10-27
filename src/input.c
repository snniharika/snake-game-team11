#include "snake.h"
#include <conio.h>
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

void initialize_input(void) {
    /* Make sure the console understands the ANSI clear-screen codes. */
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;

    if (out != INVALID_HANDLE_VALUE && GetConsoleMode(out, &mode))
        SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
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
        case 13: case ' ': return KEY_ENTER;
        case 27: return KEY_ESCAPE;
        case 'p': case 'P': return KEY_PAUSE;
        case 'w': case 'W': return KEY_UP;
        case 's': case 'S': return KEY_DOWN;
        case 'a': case 'A': return KEY_LEFT;
        case 'd': case 'D': return KEY_RIGHT;
        case 'x': case 'X': return KEY_QUIT;
        default: return KEY_NONE;
    }
}
