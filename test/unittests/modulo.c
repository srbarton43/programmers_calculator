#include "number.h"
#include "unittest_utils.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  int ret = 0;
  FILE *fp = fopen(argv[1], "r");
  if (!fp)
    return -1;

  u64 numerator_h, numerator_l, denominator_h, denominator_l, correct_h, correct_l;
  int ws;

  int test_no = 1;
  
  printf("Testing the Modulo Operator\n");

  // File format: denominator_h denominator_l numerator_h numerator_l correct_h correct_l wordsize
  while (fscanf(fp, "%llx %llx %llx %llx %llx %llx %d\n", &denominator_h,
  &denominator_l, &numerator_h, &numerator_l, &correct_h, &correct_l,
  &ws) == 7) {
  printf("Test input: numerator=%llx%llx, denominator=%llx%llx, expected=%llx%llx, ws=%d\n", 
           numerator_h, numerator_l, denominator_h, denominator_l, correct_h, correct_l, ws);
    number_t out = {0};
    number_t denominator_num = {ws, {denominator_h, denominator_l}, {0}};
    number_t numerator_num = {ws, {numerator_h, numerator_l}, {0}};
    // Function signature: modulo(number_t *out, number_t *denominator, number_t *numerator, int wordsize)
    modulo(&out, &denominator_num, &numerator_num, ws);
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
