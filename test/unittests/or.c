#include "number.h"
#include "utils.h"

int main () {
  init_numbers();
  int ret = 0;
  
  printf("Testing the OR Operator\n");
  ret = max(ret, test_or("1001", 8, "110", 8, "1111", NULL));
  ret = max(ret, test_or("1001", 8, "100", 8, "1101", NULL));
  ret = max(ret, test_or("110011001100", 16, "11", 8, "110011001111", NULL));
  
  if (!ret) printf("All Tests Passed!\n");
  else printf("Not All Tests Passed!\n");
  free_numbers();
  return ret;
}
