#include <stdio.h>
#include "number.h"
#include "utils.h"

int main() {
  int ret = 0;

  printf("Testing the Add Operator\n");
  ret = max(ret, test_add("1101", 8, "1011", 8, "11000", NULL));
  ret = max(ret, test_add("1111", 4, "1111", 8, "11110", NULL));
  ret = max(ret, test_add("1111", 4, "1111", 4, "1110", NULL));
  ret = max(ret, test_add("1100", 8, "10001", 8, "11101", NULL));
  ret = max(ret, test_add("0", 1, "10001", 8, "10001", "Adding Zero"));
  ret = max(ret, test_add("1000000", 8, "11000000", 8, "0",
                          "Adding Two's Complement"));
  ret = max(ret, test_add("1101", 8, "1", 1, "1110", "Adding 1-bit to 8-bit"));

  if (!ret)
    printf("All Tests Passed!\n");
  else
    printf("Not All Tests Passed!\n");
  return ret;
}
