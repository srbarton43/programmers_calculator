
#include "number.h"
#include "unittest_utils.h"
#include <stdio.h>
#include <stdlib.h>

static const number_t empty_number = {0};
int main() {
  number_t number;
  int ret = 0;
  new_number(&number, BINARY,
      "11111111"
      "1001100110011001100110011001100110011001100110011001100110011001",
             80);
  number_print(&number);
  number_t correct = {0, {0xff, 0x9999999999999999}, {0}};
  ret = max(ret, is_equal(&number, correct));
  number_t number2 = {80, {0xf, 0x96}, {0}};
  number_print(&number2);
  number_t correct1 = {0, {0xf, 0x96}, {0}};
  ret = max(ret, is_equal(&number2, correct1));
  char s[] = "1001100110011001100110011001100110011001100110011001100110011001";
  number = empty_number;
  new_number(&number, BINARY, s, 64);
  number_print(&number);
  number_t correct2 = {0, {0x0, 0x9999999999999999}, {0}};
  ret = max(ret, is_equal(&number, correct2));
  number = empty_number;
  new_number(&number, BINARY, "10010110", 16);
  number_print(&number);
  number_t correct3 = {0, {0x0, 0x96}, {0}};
  ret = max(ret, is_equal(&number, correct3));
  number = empty_number;
  new_number(&number, HEXADECIMAL, "dead000000000000feed", 128);
  number_print(&number);
  number_t correct4 = {0,{0xdead, 0xfeed}, {0}};
  ret = max(ret, is_equal(&number, correct4));
  if (!ret)
    printf("All Tests Passed!\n");
  else
    printf("Not All Tests Passed!\n");
  return ret;
}
