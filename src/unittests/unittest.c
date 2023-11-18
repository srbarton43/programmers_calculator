#include "number.h"
#include "utils.h"

int main (int argc, char* argv[]) {
  printf("******* Unit Tests ********\n");
  int ret = 0;
  printf("Testing the Add Operator\n");
  ret = max(0, test_add("1101", "1011", "11000", 8, NULL));
  ret = max(0, test_add("1111", "1111", "11110", 8, NULL));
  ret = max(0, test_add("1100", "10001", "11101", 8, NULL));
  ret = max(0, test_add("0", "10001", "10001", 8, "Adding Zero"));
  ret = max(0, test_add("1000000", "11000000", "0", 8, "Adding Two's Complement"));
  if (!ret) printf("All Tests Passed!\n");
  return ret;
}
