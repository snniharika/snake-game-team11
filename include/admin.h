#ifndef ADMIN_H
#define ADMIN_H

#include <stdbool.h>

/*
 * Authenticate an Administrator using the supplied password.
 *
 * Returns true if authentication succeeds.
 * Returns false if authentication fails or the credential
 * configuration cannot be read or validated.
 *
 * Passwords must never be logged or printed.
 */
bool admin_authenticate(const char *password);

/*
 * Return whether the current process has an authenticated
 * Administrator session.
 */
bool admin_is_authenticated(void);

/*
 * Invalidate the current Administrator session.
 */
void admin_logout(void);

#endif
