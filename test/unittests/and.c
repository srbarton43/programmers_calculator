#include "number.h"
#include "utils.h"

int main () {
  init_numbers();
  int ret = 0;
  
  printf("Testing the And Operator\n");
  ret = max(ret, test_and("1101", 8, "0010", 8, "0", "Adding 8-bit to 8-bit"));
  ret = max(ret, test_and("11111101", 8, "1111", 8, "1101", "Adding 8-bit to 8-bit"));
  ret = max(ret, test_and("11111111", 8, "10101010", 8, "10101010", "Adding 8-bit to 8-bit"));
  
  if (!ret) printf("All Tests Passed!\n");
  else printf("Not All Tests Passed!\n");
  free_numbers();
  return ret;
}
