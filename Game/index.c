#include <stdio.h>

enum State { Working = 1, Failed = 0 };

int main() {
  enum State currentStatus = Working;

  printf("Current state: %d", currentStatus);
  return 0;
}