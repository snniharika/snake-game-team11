#include "snake.h"
#include <stdio.h>
#include <ctype.h>

Key read_key(void) {
    int ch = getchar();

    if (ch == EOF) return KEY_QUIT;

    ch = tolower((unsigned char)ch);

    switch (ch) {
        case 'w': return KEY_UP;
        case 's': return KEY_DOWN;
        case 'a': return KEY_LEFT;
        case 'd': return KEY_RIGHT;
        case 'x': return KEY_QUIT;
        case 27:  return KEY_ESCAPE;
        case '\n':
        case '\r': return KEY_ENTER;
        default:  return KEY_NONE;
    }
}

void initialize_input(void) {}
void shutdown_input(void) {}
