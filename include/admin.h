#ifndef ADMIN_H
#define ADMIN_H

#include <stdbool.h>
#include "persistence.h"

/*
 * Authenticate an Administrator using the supplied password.
 */
bool admin_authenticate(const char *password);

/*
 * Check whether the current process has an authenticated
 * Administrator session.
 */
bool admin_is_authenticated(void);

/*
 * Log out the Administrator.
 */
void admin_logout(void);

/*
 * Display protected game data to an authenticated Administrator.
 */
bool admin_view_protected_data(void);

/*
 * Save modified protected data only for an authenticated Administrator.
 */
bool admin_save_protected_data(const ProtectedGameData *data);

#endif
