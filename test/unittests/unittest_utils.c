#include "unittest_utils.h"
#include <stdio.h>

int max(int a, int b) {
  if (a >= b)
    return a;
  return b;
}

int min(int a, int b) {
  if (a <= b)
    return a;
  return b;
}

int is_equal(number_t *number, number_t correct) {
  for (int i = 0; i < SIZE; i++) {
    if (number->num[i] != correct.num[i]) {
      printf("Test Failed\n");
      return 1;
    }
  }
  printf("Test Passed\n");
  return 0;
}
