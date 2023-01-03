#include "../include/core.h"
#include "../lib/sqlite/sqlite3.h"
#include <stdio.h>
#include <string.h>

char* select_all_users_query = "SELECT * from users;";
char* insert_user_query = "INSERT INTO users(name, enabled) VALUES (?, ?);";

core_op_result create_user(char* name, bool enabled) {
  sqlite3 *db;
  sqlite3_stmt *res;

  // Validate parameters
  if (!(strlen(name) > 0)) {
    fprintf(stderr, "Name must not be an empty string");
    return ValidationError;
  }

  int rc = sqlite3_open("test.db", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return DBError;
  }

  rc = sqlite3_prepare_v2(db, insert_user_query, -1, &res, 0);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to execute statement statement: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return DBError;
  }

  sqlite3_bind_text(res, 1, name, -1, SQLITE_STATIC);
  sqlite3_bind_int(res, 2, (enabled ? 1 : 0));

  int step = sqlite3_step(res);
  if (rc != SQLITE_DONE) {
    fprintf(stderr, "execution failed: %s\n", sqlite3_errmsg(db));
  }

  // Cleanup
  sqlite3_finalize(res);
  sqlite3_close(db);

  return Success;
}