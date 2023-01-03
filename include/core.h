/*
 * Provide basic CRUD on top of SQLite + Sessions extension
 * Author: Joshua Rowe
 */
#include <stdbool.h>

typedef struct user_t {
  char* name;
  bool enabled;
} user_t;

typedef enum core_op_result {
  Success,
  ValidationError,
  DBError
} core_op_result;

core_op_result create_user(char* name, bool enabled);