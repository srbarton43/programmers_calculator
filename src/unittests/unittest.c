#include "number.h"
#include "utils.h"

int main (int argc, char* argv[]) {
  printf("******* Unit Tests ********\n");
  int ret = 0;
  
  printf("Testing the Add Operator\n");
  ret = max(ret, test_add("1101", "1011", "11000", 8, NULL));
  ret = max(ret, test_add("1111", "1111", "11110", 8, NULL));
  ret = max(ret, test_add("1100", "10001", "11101", 8, NULL));
  ret = max(ret, test_add("0", "10001", "10001", 8, "Adding Zero"));
  ret = max(ret, test_add("1000000", "11000000", "0", 8, "Adding Two's Complement"));
  
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
  
  printf("Testing TWO's COMPLEMENT\n");
  ret = max(ret, test_twos_comp("1", "11111111", 8, "Minimum Integer"));
  ret = max(ret, test_twos_comp("1111111", "10000001", 8, "Maximum Integer"));
  ret = max(ret, test_twos_comp("0", "0", 8, "Zero to Zero"));
  ret = max(ret, test_twos_comp("11111111", "1", 8, "-1 to 1"));
  
  if (!ret) printf("All Tests Passed!\n");
  return ret;
}
