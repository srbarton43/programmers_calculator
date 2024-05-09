#include <stdio.h>
#include "number.h"
#include "utils.h"

int main() {
  int ret = 0;
  printf("Testing LSHIFT\n");
  ret = max(ret, test_lshift("110011", "1", "1100110", 8, NULL));
  ret = max(ret, test_lshift("110011", "10", "11001100", 8, NULL));
  ret = max(ret, test_lshift("110011", "11", "10011000", 8, NULL));
  ret = max(ret, test_lshift("110011", "1111", "0", 8, NULL));

  printf("Testing RSHIFT\n");
  ret = max(ret, test_rshift("110011", "1", "11001", 8, NULL));
  ret = max(ret, test_rshift("110011", "10", "1100", 8, NULL));
  ret = max(ret, test_rshift("110011", "11", "110", 8, NULL));
  ret = max(ret, test_rshift("110011", "1111", "0", 8, NULL));

  if (!ret)
    printf("All Tests Passed!\n");
  else
    printf("Not All Tests Passed!\n");
  return ret;
}
