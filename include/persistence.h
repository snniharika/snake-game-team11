#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <stdbool.h>

typedef struct {
    int high_score;
    unsigned int games_played;
    int last_score;
    char last_difficulty[16];
} ProtectedGameData;

bool persistence_load_game_data(ProtectedGameData *data);

#endif
