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
