# Sprint 2 Report — Teammate 2 (Snake Game, Team 11)

My Sprint 2 items, delivered in pull request #35 ("Add pause/resume and Game
Over screen", merged into `main` by the repo owner).

| Issue | ID | User story (as a Player, I want...) | Test case |
|---|---|---|---|
| #24 | SNK-F-010 | a Game Over screen with Restart and Exit | TC-Collision-03 |
| #26 | SNK-F-012 | to see my final score when the game ends | see note |
| #27 | SNK-F-013 | to pause and resume without losing my game | TC-Pause-01 |

Note: the issue, the SRS test table and the SRS requirement table give three
different IDs for the final-score test (TC-Score-02, TC-FinalScore-01,
TC-Score-01). The behaviour is the same.

## What was implemented

### SNK-F-013 — Pause and resume
- `GameState` gets a `paused` flag (`include/snake.h`).
- `set_paused()` in `src/game.c` sets or clears it, and refuses once the game
  is over.
- While paused, `update_game()` returns immediately and `update_direction()`
  ignores keys, so no field of the state changes.
- `run_pause_menu()` in `src/main.c` shows Resume and Main Menu. Esc or P
  opens it, and Esc, P or choosing Resume continues from the same position.
- `render_pause_menu()` in `src/render.c` draws the frozen board with a
  PAUSED label.
- `src/input.c` maps `P` to a new `KEY_PAUSE`, and Space to Enter.

### SNK-F-010 and SNK-F-012 — Game Over screen and final score
- `render_game_over()` in `src/render.c` shows "GAME OVER", the final score,
  the difficulty, the last board, and the options Restart and Exit.
- `run_game_over_menu()` in `src/main.c` lets the player choose with Up/Down
  and Enter. Restart is highlighted by default. Esc returns to the main menu
  and X exits.
- `play_game()` loops, so Restart starts a fresh game without going back to
  the menu.
- The final score cannot change on this screen, because `update_game()` does
  nothing once `game_over` is set.

## Verification
`tests/sprint2_test.c` checks that pause freezes the whole game state, that
resume keeps the same direction, that pause is refused after game over, that
the score and snake stay frozen after game over, and that a restart gives a
fresh game. Result: `SPRINT 2 TESTS PASSED`.

Manual checks to do on Windows (fill in before the demo):

| # | Check | Result |
|---|---|---|
| 1 | Esc pauses, waiting does not move the snake, Resume continues | |
| 2 | Pause then Main Menu returns to the menu | |
| 3 | Hitting a wall shows Game Over with the final score | |
| 4 | Restart starts a fresh game | |
| 5 | Exit leaves the program | |

## Deviations from the submitted SRS, SAD and Test Plan
1. Esc during play opens a pause menu (Resume / Main Menu) instead of going
   straight back to the main menu.
2. Space also confirms menu choices.
3. The final-score test ID differs between the documents.

## Files in this package
- `include/snake.h`, `src/game.c`, `src/input.c`, `src/main.c`,
  `src/render.c`: the code as merged in PR #35.
- `docs/pr35-changes.patch`: exactly what PR #35 changed.
- `tests/sprint2_test.c`: the tests above.
- `Makefile`: builds this package on its own.
