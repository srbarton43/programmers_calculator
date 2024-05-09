#include <stdio.h>
#include "number.h"
#include "utils.h"

int main() {
  int ret = 0;
  printf("Testing SUB\n");
  ret = max(ret, test_sub("1", 8, "10", 8, "1", "2-1 = 1"));
  ret = max(ret,
            test_sub("11111111", 8, "11111110", 8, "11111111", "63-64 = -1"));
  ret = max(ret, test_sub("1111", 8, "1111", 8, "0", "15-15 = 0"));
  ret = max(ret, test_sub("1", 4, "1111", 4, "1110", "15-1=14"));

  if (!ret)
    printf("All Tests Passed!\n");
  else
    printf("Not All Tests Passed!\n");
  return ret;
}
