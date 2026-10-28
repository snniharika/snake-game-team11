#include "admin.h"

#include <windows.h>
#include <bcrypt.h>
#include <limits.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ADMIN_CONFIG_PATH "config/admin_credentials.conf"

#define SALT_LENGTH 16
#define HASH_LENGTH 32

#define HEX_SALT_LENGTH (SALT_LENGTH * 2)
#define HEX_HASH_LENGTH (HASH_LENGTH * 2)

#define MAX_CONFIG_LINE 256
#define MIN_ITERATIONS 100000
#define MAX_ITERATIONS 2000000

static bool authenticated = false;

typedef struct {
    unsigned char salt[SALT_LENGTH];
    unsigned char hash[HASH_LENGTH];
    unsigned long iterations;
} AdminCredentials;


/*
 * Convert a hexadecimal character to its numeric value.
 * Returns -1 for invalid characters.
 */
static int hex_value(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';

    ch = (char)tolower((unsigned char)ch);

    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;

    return -1;
}


/*
 * Convert a hexadecimal string to bytes.
 */
static bool hex_to_bytes(
    const char *hex,
    unsigned char *output,
    size_t output_length)
{
    if (hex == NULL || output == NULL)
        return false;

    if (strlen(hex) != output_length * 2)
        return false;

    for (size_t i = 0; i < output_length; i++) {
        int high = hex_value(hex[i * 2]);
        int low = hex_value(hex[i * 2 + 1]);

        if (high < 0 || low < 0)
            return false;

        output[i] = (unsigned char)((high << 4) | low);
    }

    return true;
}


/*
 * Load the locally stored Administrator credentials.
 *
 * Expected file format:
 *
 * iterations=600000
 * salt=<32 hexadecimal characters>
 * hash=<64 hexadecimal characters>
 *
 * The actual values must be generated separately.
 */
static bool load_credentials(AdminCredentials *credentials)
{
    if (credentials == NULL)
        return false;

    FILE *file = fopen(ADMIN_CONFIG_PATH, "r");

    if (file == NULL)
        return false;

    char line[MAX_CONFIG_LINE];

    bool found_iterations = false;
    bool found_salt = false;
    bool found_hash = false;

    unsigned long iterations = 0;

    char salt_hex[HEX_SALT_LENGTH + 1] = {0};
    char hash_hex[HEX_HASH_LENGTH + 1] = {0};

    while (fgets(line, sizeof(line), file) != NULL) {
        /*
         * Reject lines that do not fit in the buffer.
         */
        if (strchr(line, '\n') == NULL && !feof(file)) {
            fclose(file);
            return false;
        }

        line[strcspn(line, "\r\n")] = '\0';

        if (strncmp(line, "iterations=", 11) == 0) {
            char *end = NULL;

            if (found_iterations) {
                fclose(file);
                return false;
            }

            iterations = strtoul(line + 11, &end, 10);

            if (end == line + 11 || *end != '\0') {
                fclose(file);
                return false;
            }

            found_iterations = true;
        }
        else if (strncmp(line, "salt=", 5) == 0) {
            if (found_salt ||
                strlen(line + 5) != HEX_SALT_LENGTH) {
                fclose(file);
                return false;
            }

            strcpy(salt_hex, line + 5);
            found_salt = true;
        }
        else if (strncmp(line, "hash=", 5) == 0) {
            if (found_hash ||
                strlen(line + 5) != HEX_HASH_LENGTH) {
                fclose(file);
                return false;
            }

            strcpy(hash_hex, line + 5);
            found_hash = true;
        }
        else if (line[0] != '\0') {
            /*
             * Reject unexpected configuration entries.
             */
            fclose(file);
            return false;
        }
    }

    bool read_error = ferror(file) != 0;
    fclose(file);

    if (read_error ||
        !found_iterations ||
        !found_salt ||
        !found_hash) {
        return false;
    }

    if (iterations < MIN_ITERATIONS ||
        iterations > MAX_ITERATIONS) {
        return false;
    }

    if (!hex_to_bytes(
            salt_hex,
            credentials->salt,
            SALT_LENGTH)) {
        return false;
    }

    if (!hex_to_bytes(
            hash_hex,
            credentials->hash,
            HASH_LENGTH)) {
        return false;
    }

    credentials->iterations = iterations;

    return true;
}


/*
 * Derive a password hash using PBKDF2-HMAC-SHA-256.
 */
static bool derive_password_hash(
    const char *password,
    const unsigned char *salt,
    unsigned long iterations,
    unsigned char *output)
{
    if (password == NULL || salt == NULL || output == NULL)
        return false;

    size_t password_length = strlen(password);

    if (password_length == 0 ||
        password_length > 1024 ||
        password_length > ULONG_MAX) {
        return false;
    }

    BCRYPT_ALG_HANDLE algorithm = NULL;

    NTSTATUS status = BCryptOpenAlgorithmProvider(
        &algorithm,
        BCRYPT_SHA256_ALGORITHM,
        NULL,
        BCRYPT_ALG_HANDLE_HMAC_FLAG
    );

    if (status < 0)
        return false;

    status = BCryptDeriveKeyPBKDF2(
        algorithm,
        (PUCHAR)password,
        (ULONG)password_length,
        (PUCHAR)salt,
        SALT_LENGTH,
        (ULONGLONG)iterations,
        output,
        HASH_LENGTH,
        0
    );

    BCryptCloseAlgorithmProvider(algorithm, 0);

    return status >= 0;
}


/*
 * Compare two byte arrays without returning early on
 * the first differing byte.
 */
static bool constant_time_equal(
    const unsigned char *first,
    const unsigned char *second,
    size_t length)
{
    unsigned char difference = 0;

    for (size_t i = 0; i < length; i++)
        difference |= first[i] ^ second[i];

    return difference == 0;
}


/*
 * Authenticate the Administrator.
 */
bool admin_authenticate(const char *password)
{
    /*
     * A new authentication attempt must not retain an
     * earlier authenticated state after a failure.
     */
    authenticated = false;

    if (password == NULL)
        return false;

    AdminCredentials credentials = {0};
    unsigned char calculated_hash[HASH_LENGTH] = {0};

    if (!load_credentials(&credentials))
        return false;

    if (!derive_password_hash(
            password,
            credentials.salt,
            credentials.iterations,
            calculated_hash)) {
        SecureZeroMemory(&credentials, sizeof(credentials));
        SecureZeroMemory(calculated_hash, sizeof(calculated_hash));
        return false;
    }

    bool matches = constant_time_equal(
        calculated_hash,
        credentials.hash,
        HASH_LENGTH
    );

    SecureZeroMemory(&credentials, sizeof(credentials));
    SecureZeroMemory(calculated_hash, sizeof(calculated_hash));

    authenticated = matches;

    return matches;
}


/*
 * Check whether the current process has an authenticated
 * Administrator session.
 */
bool admin_is_authenticated(void)
{
    return authenticated;
}


/*
 * Log out the Administrator.
 */
void admin_logout(void)
{
    authenticated = false;
}
