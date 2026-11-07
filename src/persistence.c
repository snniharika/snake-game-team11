#include "persistence.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define GAME_DATA_PATH "data/game_data.txt"

static void initialize_empty_data(ProtectedGameData *data)
{
    data->high_score = 0;
    data->games_played = 0;
    data->last_score = 0;

    strcpy(data->last_difficulty, "Easy");
}

static bool valid_difficulty(const char *difficulty)
{
    return strcmp(difficulty, "Easy") == 0 ||
           strcmp(difficulty, "Medium") == 0 ||
           strcmp(difficulty, "Hard") == 0;
}

bool persistence_load_game_data(ProtectedGameData *data)
{
    if (data == NULL) {
        return false;
    }

    initialize_empty_data(data);

    FILE *file = fopen(GAME_DATA_PATH, "r");

    if (file == NULL) {
        return true;
    }

    int fields_read = fscanf(
        file,
        "high_score=%d\n"
        "games_played=%u\n"
        "last_score=%d\n"
        "last_difficulty=%15s",
        &data->high_score,
        &data->games_played,
        &data->last_score,
        data->last_difficulty
    );

    int ch;
    bool extra_content = false;

    while ((ch = fgetc(file)) != EOF) {
        if (!isspace((unsigned char)ch)) {
            extra_content = true;
            break;
        }
    }

    bool read_error = ferror(file) != 0;

    fclose(file);

    if (fields_read != 4 ||
        extra_content ||
        read_error ||
        data->high_score < 0 ||
        data->last_score < 0 ||
        data->games_played > 2147483647U ||
        !valid_difficulty(data->last_difficulty)) {

        initialize_empty_data(data);
        return false;
    }

    return true;
}




/*
 * Save updated game data to the local configuration file.
 */
bool persistence_save_game_data(const ProtectedGameData *data)
{
    if (data == NULL) {
        return false;
    }

    /*
     * Validate values before writing them.
     */
    if (data->high_score < 0 ||
        data->last_score < 0 ||
        data->games_played > 2147483647U) {
        return false;
    }

    if (strcmp(data->last_difficulty, "Easy") != 0 &&
        strcmp(data->last_difficulty, "Medium") != 0 &&
        strcmp(data->last_difficulty, "Hard") != 0) {
        return false;
    }

    FILE *file = fopen(GAME_DATA_PATH, "w");

    if (file == NULL) {
        return false;
    }

    int result = fprintf(
        file,
        "high_score=%d\n"
        "games_played=%u\n"
        "last_score=%d\n"
        "last_difficulty=%s\n",
        data->high_score,
        data->games_played,
        data->last_score,
        data->last_difficulty
    );

    bool success = result >= 0 && fflush(file) == 0;

    if (fclose(file) != 0) {
        success = false;
    }

    return success;
}
