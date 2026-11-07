#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <stdbool.h>

typedef struct {
    int high_score;
    unsigned int games_played;
    int last_score;
    char last_difficulty[16];
} ProtectedGameData;

/* Load stored game data from the local file. */
bool persistence_load_game_data(ProtectedGameData *data);

/* Save updated game data to the local file. */
bool persistence_save_game_data(const ProtectedGameData *data);

#endif
