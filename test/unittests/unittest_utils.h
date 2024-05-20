#ifndef _UNITTEST_UTILS_H
#define _UNITTEST_UTILS_H
#include "number.h"

// bitfield for number metadata
struct md_bf {
  unsigned int UNSIGNED_OVERFLOW : 1;
  unsigned int SIGNED_OVERFLOW : 1;
};

typedef struct number {
  int wordsize;          // wordsize for the bitstring
  u64 num[SIZE];         // stores bitstring (only conisider [wordsize] LSB's
  struct md_bf metadata; // stores number metadata about overflow, etc
} number_t;

// return max(a,b)
int max(int a, int b);

// return min(a,b)
int min(int a, int b);

int is_equal(number_t *number, number_t correct);

#endif
