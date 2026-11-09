#include "snake.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    GameState g;

    /* SNK-F-013: pause freezes the whole state; resume continues */
    initialize_game(&g, MEDIUM);
    update_game(&g); update_game(&g);
    GameState before = g;
    set_paused(&g, true);
    assert(g.paused);
    for (int i = 0; i < 20; i++) { update_direction(&g, KEY_UP); update_game(&g); }
    g.paused = before.paused;
    assert(memcmp(&g, &before, sizeof g) == 0);
    set_paused(&g, true);
    set_paused(&g, false);
    assert(!g.paused);
    Position h0 = g.segments[0];
    update_game(&g);
    assert(g.segments[0].x == h0.x + 1 && g.segments[0].y == h0.y);

    /* pause cannot be set once the game is over */
    initialize_game(&g, EASY);
    for (int i = 0; i < BOARD_WIDTH && !g.game_over; i++) update_game(&g);
    assert(g.game_over);
    set_paused(&g, true);
    assert(!g.paused);

    /* SNK-F-010 / SNK-F-012: score is frozen once the game is over */
    int final_score = g.score;
    Position head = g.segments[0];
    for (int i = 0; i < 10; i++) update_game(&g);
    assert(g.score == final_score);
    assert(g.segments[0].x == head.x && g.segments[0].y == head.y);

    /* a restart gives a fresh game */
    initialize_game(&g, EASY);
    assert(!g.game_over && !g.paused && g.score == 0 && g.length == INITIAL_LENGTH);

    puts("SPRINT 2 TESTS PASSED");
    return 0;
}
