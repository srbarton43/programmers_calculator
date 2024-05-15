#ifndef _UNITTEST_UTILS_H
#define _UNITTEST_UTILS_H
#include "number.h"

// return max(a,b)
int max(int a, int b);

// return min(a,b)
int min(int a, int b);

// 0 if equal
int is_equal(number_t *num, number_t correct);

#endif
