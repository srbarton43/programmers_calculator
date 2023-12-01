#include "number.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char* argv[]) {
  init_numbers();
  int ret = 0;
  
  printf("Decimal to Binary Conversion Test\n");
  
  char expected[65];
  unsigned long long decimal;

  FILE* fp = fopen(argv[argc-1], "r");

  if (!fp) exit(1);
  
  while (fscanf(fp, "%llu %s\n", &decimal, expected) == 2) {
    printf("foo\n");
    ret = max(ret, test_dec2binary(decimal, expected));
  }
    
  if (!ret) printf("All Tests Passed!\n");
  else printf("Not All Tests Passed!\n");
  free_numbers();
  return ret;
}
