#include "number.h"
#include "unittest_utils.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  int ret = 0;
  FILE *fp = fopen(argv[1], "r");
  if (!fp)
    return -1;

  u64 divisor_h, divisor_l, dividend_h, dividend_l, correct_h, correct_l;
  int ws;

  int test_no = 1;
  
  printf("Testing the Divide Operator\n");

  // File format: divisor_h divisor_l dividend_h dividend_l correct_h correct_l wordsize
  // Run only the 6th test case
  while (fscanf(fp, "%llx %llx %llx %llx %llx %llx %d\n", &divisor_h,
  &divisor_l, &dividend_h, &dividend_l, &correct_h, &correct_l,
  &ws) == 7) {
  // Run all tests now
  printf("Test input: divisor=%llx%llx, dividend=%llx%llx, expected=%llx%llx, ws=%d\n", 
           divisor_h, divisor_l, dividend_h, dividend_l, correct_h, correct_l, ws);
    number_t out = {0};
    number_t dividend_num = {ws, {dividend_h, dividend_l}, {0}};
    number_t divisor_num = {ws, {divisor_h, divisor_l}, {0}};
    // Function signature: divide(number_t *out, number_t *divisor, number_t *dividend, int wordsize)
    divide(&out, &divisor_num, &dividend_num, ws);
    if (out.num[0] != correct_h || out.num[1] != correct_l) {
      printf("[%02d] TEST FAILED\n", test_no);
      ret++;
    } else {
      printf("[%02d] TEST PASSED\n", test_no);
    }
    number_print(&out);
    test_no++;
  }


  if (!ret)
    printf("All Tests Passed!\n");
  else
    printf("Not All Tests Passed!\n");
  return ret;
}
