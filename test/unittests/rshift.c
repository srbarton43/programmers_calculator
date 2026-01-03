#include <stdio.h>
#include <stdlib.h>
#include "number.h"
#include "unittest_utils.h"

int numbers_equal(number_t *test, u64 correct_h, u64 correct_l);
int numbers_equal_metadata(number_t *test, u64 correct_h, u64 correct_l, unsigned short correct_bm);
int construct_number(number_t *out, int wordsize, u64 msb, u64 lsb);

int main(int argc, char *argv[]) {
  printf("Testing LSHIFT\n");
  int ret = 0;
  FILE *fp;
  printf("argc=%d\n", argc);
  printf("argv=%s\n", argv[1]);
  fp = fopen(argv[1], "r");
  if (!fp) exit(1);
  u64 i1, i0, o1, o0;
  int shift, wordsize;
  number_t *shifted, *num, *shift_num;
  shifted = num = shift_num = 0;
  number_alloc(&shifted);
  number_alloc(&num);
  number_alloc(&shift_num);
  while (fscanf(fp, "0x%llx 0x%llx 0x%llx 0x%llx %d %d\n", &i1, &i0, &o1, &o0, &shift, &wordsize) == 6) {
    printf("0x%llx 0x%llx 0x%llx 0x%llx %d %d\n", i1, i0, o1, o0, shift, wordsize);
    construct_number(num, wordsize, i1, i0);
    printf("input\n");
    number_print(stdout, num);
    printf("%d\n", __LINE__);
    construct_number(shift_num, 8, 0, shift);
    rshift(shifted, num, shift_num, wordsize);
    printf("output\n");
    number_print(stdout, shifted);
    if (numbers_equal(shifted, o1, o0)) {
      printf("Test Passed\n");
    } else {
      printf("Test Failed\n");
      ret = 1;
    }
  }
  fclose(fp);
  return ret;
}
