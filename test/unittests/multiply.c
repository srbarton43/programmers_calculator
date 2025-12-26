#include "number.h"
#include "unittest_utils.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  int ret = 0;
  FILE *fp = fopen(argv[1], "r");
  if (!fp)
    return -1;

  u64 b_h, b_l, a_h, a_l, correct_h, correct_l;
  int ws;

  int test_no = 1;
  
  printf("Testing the Multiply Operator\n");

  // File format: denominator_h denominator_l numerator_h numerator_l correct_h correct_l wordsize
  while (fscanf(fp, "%llx %llx %llx %llx %llx %llx %d\n", &a_h,
  &a_l, &b_h, &b_l, &correct_h, &correct_l,
  &ws) == 7) {
  printf("Test input: a=%llx%llx, b=%llx%llx, expected=%llx%llx, ws=%d\n", 
           a_h, a_l, b_h, b_l, correct_h, correct_l, ws);
    number_t out = {0};
    number_t a_num = {ws, {a_h, a_l}, {0}};
    number_t b_num = {ws, {b_h, b_l}, {0}};
    // Function signature: multiply(number_t *out, number_t *a, number_t *b, int wordsize)
    multiply(&out, &a_num, &b_num, ws);
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
