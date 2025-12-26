#include <stdio.h>
#include <stdlib.h>
#include "number.h"
#include "unittest_utils.h"
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
  while (fscanf(fp, "0x%llx 0x%llx 0x%llx 0x%llx %d %d\n", &i1, &i0, &o1, &o0, &shift, &wordsize) == 6) {
    printf("0x%llx 0x%llx 0x%llx 0x%llx %d %d\n", i1, i0, o1, o0, shift, wordsize);
    number_t shifted = {0};
    number_t num = {wordsize, {i1, i0}, {0} };
    printf("input\n");
    number_print(&num);
    printf("%d\n", __LINE__);
    number_t shift_num = {8, {0, shift}, {0}};
    rshift(&shifted, &num, &shift_num, wordsize);
    printf("output\n");
    number_print(&shifted);
    if (shifted.num[0] == o1 && shifted.num[1] == o0) {
      printf("Test Passed\n");
    } else {
      printf("Test Failed\n");
      ret = 1;
    }
  }
  fclose(fp);
  return ret;
}
