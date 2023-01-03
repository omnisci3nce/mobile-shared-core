#include "../include/core.h"

int main() {
  init_table();

  create_user("Josh", true);
  create_user("Admin", false);

  users_result result = get_users(10, 0);
  printf("%d users found\n", result.len);

  for (int i = 0; i < result.len; i++) {
    printf("User [name='%s'] [enabled=%s]\n", result.users[i].name, result.users[i].enabled ? "true" : "false");
  }

  return 0;
}