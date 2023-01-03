#include "../include/core.h"
#include "../lib/sqlite/sqlite3.h"
#include <stdio.h>
#include <string.h>

char* create_table_query = "CREATE TABLE 'users' ('name'	TEXT, 'enabled'	INTEGER, PRIMARY KEY('name'))";
char* select_all_users_query = "SELECT * from users LIMIT 10;";
char* insert_user_query = "INSERT INTO users(name, enabled) VALUES (?, ?);";

int init_table() {
  sqlite3 *db;
  char *err_msg;

  int rc = sqlite3_open("test.db", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  rc = sqlite3_exec(db, create_table_query, 0, 0, &err_msg);

  if (rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);        
    sqlite3_close(db);
    return 1;
  } 

  sqlite3_close(db);
  
  return 0;
}

users_result get_users(int limit, int offset) {
  sqlite3 *db;
  sqlite3_stmt *res;

  int rc = sqlite3_open("test.db", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
  }

  rc = sqlite3_prepare_v2(db, select_all_users_query, -1, &res, 0);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
  }

  users_result result;
  // Allocate space for our results
  result.users = malloc((sizeof(user_t)) * limit);

  int i = 0;
  while (i < limit) {
    int step = sqlite3_step(res);
    if (step == SQLITE_DONE) {
      break;
    } else {
      const char* name = sqlite3_column_text(res, 0);
      char *copy = malloc(strlen(name) + 1);
      strcpy(copy, name); // name will be automatically freed next sqlite3_step so we need to copy it out
      result.users[i].name = copy;
      result.users[i].enabled = sqlite3_column_int(res, 1);   
      i++;
    }
  }

  result.len = i;

  // Cleanup
  sqlite3_finalize(res);
  sqlite3_close(db);

  return result;
}

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
    fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
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