
#include "persistence.h"
#include "admin.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>

#define GAME_DATA_PATH "data/game_data.txt"
#define MAX_GAMES_PLAYED 2147483647U

static void initialize_empty_data(ProtectedGameData *data)
{
    data->high_score = 0;
    data->games_played = 0;
    data->last_score = 0;

    strcpy(data->last_difficulty, "Easy");
}

static bool valid_difficulty(const char *difficulty)
{
    if (difficulty == NULL) {
        return false;
    }

    return strcmp(difficulty, "Easy") == 0 ||
           strcmp(difficulty, "Medium") == 0 ||
           strcmp(difficulty, "Hard") == 0;
}

static bool valid_game_data(const ProtectedGameData *data)
{
    if (data == NULL) {
        return false;
    }

    if (data->high_score < 0 ||
        data->last_score < 0 ||
        data->games_played > MAX_GAMES_PLAYED) {
        return false;
    }

    if (memchr(data->last_difficulty, '\0',
               sizeof(data->last_difficulty)) == NULL) {
        return false;
    }

    return valid_difficulty(data->last_difficulty);
}

/*
 * Internal function.
 * Reads stored data without exposing it through the public API.
 */
static bool load_game_data_internal(ProtectedGameData *data)
{
    if (data == NULL) {
        return false;
    }

    ProtectedGameData loaded_data;
    initialize_empty_data(&loaded_data);

    errno = 0;
    FILE *file = fopen(GAME_DATA_PATH, "r");

    if (file == NULL) {
        int open_error = errno;

        if (open_error == ENOENT) {
            *data = loaded_data;
            return true;
        }

        return false;
    }

    int fields_read = fscanf(
        file,
        "high_score=%d\n"
        "games_played=%u\n"
        "last_score=%d\n"
        "last_difficulty=%15s",
        &loaded_data.high_score,
        &loaded_data.games_played,
        &loaded_data.last_score,
        loaded_data.last_difficulty
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
    bool close_error = fclose(file) != 0;

    if (fields_read != 4 ||
        extra_content ||
        read_error ||
        close_error ||
        !valid_game_data(&loaded_data)) {
        return false;
    }

    *data = loaded_data;
    return true;
}

/*
 * Internal function.
 * Writes validated data to the storage file.
 */
static bool save_game_data_internal(const ProtectedGameData *data)
{
    if (!valid_game_data(data)) {
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

/*
 * Public administrative operation.
 * Only authenticated administrators may read protected game data.
 */
bool persistence_load_game_data(ProtectedGameData *data)
{
    if (!admin_is_authenticated()) {
        return false;
    }

    if (data == NULL) {
        return false;
    }

    return load_game_data_internal(data);
}

/*
 * Public administrative operation.
 * Only authenticated administrators may directly modify protected data.
 */
bool persistence_save_game_data(const ProtectedGameData *data)
{
    if (!admin_is_authenticated()) {
        return false;
    }

    return save_game_data_internal(data);
}

/*
 * Gameplay operation.
 * Allows the game to record a completed game without exposing
 * the protected data-loading and data-saving functions to players.
 */
bool persistence_record_completed_game(int score, const char *difficulty)
{
    if (score < 0 || !valid_difficulty(difficulty)) {
        return false;
    }

    ProtectedGameData data;

    if (!load_game_data_internal(&data)) {
        return false;
    }

    if (data.games_played >= MAX_GAMES_PLAYED) {
        return false;
    }

    data.games_played++;
    data.last_score = score;

    if (score > data.high_score) {
        data.high_score = score;
    }

    strcpy(data.last_difficulty, difficulty);

    return save_game_data_internal(&data);
}
