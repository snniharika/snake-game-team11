
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
bool persistence_save_game_data(const ProtectedGameData *data);

/* Records a completed game without exposing protected data to players. */
bool persistence_record_completed_game(int score, const char *difficulty);

#endif
