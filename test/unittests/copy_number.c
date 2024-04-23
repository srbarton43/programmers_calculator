#include "number.h"
#include "utils.h"

int main() {
  init_numbers();
  int ret = 0;
  printf("Testing COPY_NUMBER\n");

  ret = max(ret, test_copy_number("1101", 8, 8, "1101", NULL));
  ret = max(ret, test_copy_number("1101", 4, 8, "1101", NULL));
  ret = max(ret, test_copy_number("1101", 4, 2, "01", NULL));

  if (!ret)
    printf("All Tests Passed!\n");
  else
    printf("Not All Tests Passed!\n");
  free_numbers();
  return ret;
}
