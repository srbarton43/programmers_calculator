#ifndef _UNITTEST_UTILS_H
#define _UNITTEST_UTILS_H
#include "number.h"

// Use the number_t definition from number.h

// return max(a,b)
int max(int a, int b);

// return min(a,b)
int min(int a, int b);

int is_equal(number_t *number, number_t correct);

#endif
