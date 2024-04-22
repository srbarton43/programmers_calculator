#include "number.h"
#include "utils.h"

int main() {
  init_numbers();
  int ret = 0;
  printf("Testing TWO's COMPLEMENT\n");
  ret = max(ret, test_twos_comp("1", "11111111", 8, "Minimum Integer"));
  ret = max(ret, test_twos_comp("1111111", "10000001", 8, "Maximum Integer"));
  ret = max(ret, test_twos_comp("0", "0", 8, "Zero to Zero"));
  ret = max(ret, test_twos_comp("11111111", "1", 8, "-1 to 1"));
  ret = max(ret, test_twos_comp("1", "1111111111111111", 16, "16-bit word"));

  if (!ret)
    printf("All Tests Passed!\n");
  else
    printf("Not All Tests Passed!\n");
  free_numbers();
  return ret;
}
