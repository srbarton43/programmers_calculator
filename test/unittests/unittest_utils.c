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

int is_equal(number_t *num, number_t correct) {
  for (int i = 0; i < SIZE; i++) {
    if (num->num[i] != correct.num[i]) {
      printf("CASE FAILED\n");
      return 1;
    }
  }
  printf("CASE PASSED\n");
  return 0;
}
