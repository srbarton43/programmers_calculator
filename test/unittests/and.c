#include "number.h"
#include "utils.h"

int main () {
  init_numbers();
  int ret = 0;
  
  printf("Testing the And Operator\n");
  ret = max(ret, test_and("1101", 8, "0010", 8, "0", NULL));
  ret = max(ret, test_and("11111101", 8, "1111", 8, "1101", NULL));
  ret = max(ret, test_and("11111111", 8, "10101010", 8, "10101010", NULL));
  ret = max(ret, test_and("110011001100", 16, "1111", 4, "1100", NULL));
  ret = max(ret, test_and("1111", 4, "110011001100", 16, "1100", "same but reversed"));
  
  if (!ret) printf("All Tests Passed!\n");
  else printf("Not All Tests Passed!\n");
  free_numbers();
  return ret;
}
