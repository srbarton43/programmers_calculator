#include "number.h"
#include "unittest_utils.h"
#include <stdint.h>
#include <stdio.h>

int main() {
  number_t number = {128, {0, 0b1111}, {0}};
  number_t six_four = {64, {0, 64}, {0}};
  print_decimal(&number);
  printf("\n");
  print_decimal(&six_four);
  printf("\n");
  lshift(&number, &number, &six_four, 128);
  print_decimal(&number);
  printf("\n");
  number_t one = {128, {0, 1}, {0}};
  print_decimal(&one);
  printf("\n");
  ones_comp(&one, &one, 128);
  print_decimal(&one);
  printf("\n");

  number_t max_u64 = {64, {UINT32_MAX, UINT64_MAX}, {0}};
  print_decimal(&max_u64);
  printf("\n");

  number_t max_max = {64, {0xffffffffff, UINT64_MAX}, {0}};
  print_decimal(&max_max);
  printf("\n");

  number_t mm;
  new_number(&mm, HEXADECIMAL, "999999999999999999999999999999999", 128);
  print_decimal(&mm);
  printf("\n");
  number_print(&mm);
  
  new_number(&mm, BINARY, "10011101110001011010110110101000001010110111000010110101100111011111000000011111111111111111111111111111111", 128);
  print_decimal(&mm);
  printf("\n");
  number_print(&mm);

  number_t negative = {4, {0, 0b1111}, {0}};
  print_decimal(&negative);
  printf("\n");
  print_signed_decimal(&negative);
  printf("\n");
  
  number_t positive = {8, {0, 0b1111}, {0}};
  print_decimal(&positive);
  printf("\n");
  print_signed_decimal(&positive);
  printf("\n");
  
  number_t negative2 = {8, {0, 0x80}, {0}};
  print_decimal(&negative2);
  printf("\n");
  print_signed_decimal(&negative2);
  printf("\n");
}
