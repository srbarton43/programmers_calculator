
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/number.h"
#include "../include/utils.h"

static unsigned long binary2dec(char* binary);
static char* binary2hex(char* binary);
static void dec2binary(unsigned long decimal, char* binary);
static void hex2binary(char* binary, char* hex);
static void refresh_number (number_t* number);
static void calcLength(number_t* number);
static void printBits(number_t* num);

number_t* new_number(type_e type, char* number, int wordsize) {
  if (number == NULL) {
    perror("null number string");
    return NULL;
  }
  number_t* new_num = malloc(sizeof(number_t));
  new_num->wordsize = wordsize;
  new_num->bits = malloc(wordsize*sizeof(char));
  // zero bits
  for (int i = 0; i < wordsize; i++) {
    new_num->bits[i] = '0';
  }
  int slen = strlen(number);
  switch (type) {
    case BINARY:
      for (int i = 1; i <= wordsize; i++) {
        if (i <= slen)
          new_num->bits[wordsize-i] = number[slen-i];
        else
          new_num->bits[wordsize-i] = '0';
      }
      int i = wordsize-1;
      while (i >= 0 ) {
        if (new_num->bits[i] == '1')
          new_num->len = wordsize - i;
        i--;
      }
      break;
    default:
      perror("new_number: not a valid number type");
      free(new_num);
      return NULL;
  }
  return new_num;
}

/********* binary2dec *************/
/* calculates the unsigned decimal for bitstring
 *
 * parameters:
 *  char* binary => bitstring
 * returns:
 *  int => unsigned decimal given by bitstring
 *
 * We Assume:
 *  binary is non-negative and contains only 1's and 0's
 */
static unsigned long binary2dec(char* binary) {
  if (binary == NULL) {
    fprintf(stderr, "binary2dec: null bitstring\n");
    return 0;
  }
  char bin[strlen(binary)+1];
  strcpy(bin, binary);
  int pointer = 0;
  unsigned long value = 0;
  while (pointer < strlen(bin)) {
    char bit = bin[strlen(bin)-pointer-1];
    printf("bit: %c\n", bit);
    if (bit != '1' && bit != '0') {
      fprintf(stderr, "binary2dec: invalid bitstring (non binary char)\n");
      return 0;
    }
    if (bit == '1') {
      value += 1 << pointer;
    }
    pointer++;
  }
  return value;
}

/********* binary2hex *************/
/* calculates the unsigned decimal for bitstring
 *
 * parameters:
 *  char* hex => hexstring
 *  char* binary => bitstring
 * returns:
 *  nothing
 *
 * We Assume:
 *  binary is non-negative and contains only 1's and 0's
 */
static char* binary2hex (char* binary) {
  if (binary == NULL) {
    fprintf(stderr, "binary2dec: null bitstring\n");
  }
  int hex_len = strlen(binary)/4+1;
  char hex_arr[hex_len];
  int bitPointer = 0;
  int bitValue = 0;
  int nibblePointer = 0;
  int nibbleVal = 0;
  while(bitPointer < strlen(binary)) {
    if (bitValue == 4) {
      if (nibbleVal < 10) {
        hex_arr[hex_len - nibblePointer - 1] = nibbleVal + '0';
      } else {
        hex_arr[hex_len - nibblePointer - 1] = nibbleVal - 10 + 'a';
      }
      bitValue = 0;
      nibbleVal = 0;
      nibblePointer++;
    }
    char bit = binary[strlen(binary)-bitPointer-1];
    if (bit != '1' && bit != '0') {
      fprintf(stderr, "binary2hex: invalid bitstring (non bit char)\n");
      return NULL;
    }
    if (bit == '1') {
      nibbleVal += 1 << bitValue;
    }
    bitValue++;
    bitPointer++;
  }
  if (nibbleVal < 10) {
    hex_arr[hex_len - nibblePointer - 1] = nibbleVal + '0';
  } else {
    hex_arr[hex_len - nibblePointer - 1] = nibbleVal - 10 + 'a';
  }
  char* hex = malloc((hex_len + 1) * sizeof(char));
  strcpy(hex, hex_arr);
  return hex;
}

/************* dec2binary **************/
/*
 * Converts unsigned decimal to bitstring
 *
 * uses divideBy2 algorithm
 *
 * parameters:
 *  decimal => unsigned decimal value
 *  binary => bitstring to return
 * returns:
 *  none
 */
void dec2binary(unsigned long decimal, char* binary) {
  if (binary == NULL) {
    fprintf(stderr, "dec2binary: bitstring");
    return;
  }
  char bits_reversed[65];
  int pointer = 0;
  while (decimal != 0) {
    int rem = decimal % 2;
    bits_reversed[pointer] = rem + '0';
    pointer++;
    decimal = decimal >> 1;
  }
  char bitstring[pointer];
  int i = 0;
  while (pointer >= 0) {
    bitstring[i] = bits_reversed[pointer-1];
    i++;
    pointer--;
  }
  bitstring[i] = '\0';
  strcpy(binary, bitstring);
}

static void hex2binary(char* binary, char* hex) {
  if (hex == NULL) {
    fprintf(stderr, "hex2binary: null hexstring passed\n");
    return;
  }
  int partial;
  char MSNib = hex[0];
  if (MSNib == '8' || MSNib == '9' || (MSNib >= 'a' && MSNib <= 'f')) {
    partial = 0;
  } else if (MSNib <= '7' && MSNib >= '5') {
    partial = 3;
  } else if (MSNib <= '4' && MSNib >= '2') {
    partial = 2;
  } else if (MSNib == '1' || MSNib == '0'){
    partial = 1;
  } else {
    fprintf(stderr, "hex2binary: invalid MSB\n");
    return;
  }
  int bitLength = strlen(hex)*4 + partial + 1;
  char bitstring[bitLength];
  bitstring[bitLength-1] = '\0';
  int bitPointer = bitLength - 2;
  int hexPointer = strlen(hex) - 1;
  while (hexPointer >= 0) {
    int nibbleVal;
    char nibble = hex[hexPointer];
    if ((nibble > 'f' || nibble < 'a') && !(nibble < '0' || nibble > '9')) {
      fprintf(stderr, "hex2binary: invalid hex character\n");
      return;
    }
    if (nibble >= 'a') {
      nibbleVal = nibble - 'a' + 10;
    } else {
      nibbleVal = nibble - '0';
    }
    while (nibbleVal != 0) {
      int rem = nibbleVal % 2;
      bitstring[bitPointer] = rem + '0';
      bitPointer--;
      nibbleVal = nibbleVal >> 1;
    }
    hexPointer--;
  }
  strcpy(binary, bitstring);
}

void delete_number (number_t* number) {
  free(number->bits);
  free(number);
}

void number_print(number_t* number) {
  printf("--------------\n");
  printf("NUMBER %p\n", number);
  printf("WORDSIZE %d\n", number->wordsize);
  printf("LENGTH %d\n", number->len);
  char* bs = number->bits;
  printf("BITSTRING ");
  if (number->len == 0) {
    printf("0"); 
  } else {
    for (int i = number->wordsize - number->len; i < number->wordsize; i++) {
      printf("%c", bs[i]);
    }
  }
  printf("\n");
  printf("RAW ");
  for (int i = 0; i < number->wordsize; i++)
    printf("%c", bs[i]);
  printf("\n");
  printf("--------------\n");
}

static void printBits(number_t* num) {
  char* bs = num->bits;
  if (num->len == 0) {
    printf("0"); 
  } else {
    for (int i = num->wordsize - num->len; i < num->wordsize; i++) {
      printf("%c", bs[i]);
    }
  }
}

static number_t* copy_number(number_t* number) {
  number_t* new_num = malloc(sizeof(number_t));
  new_num->wordsize = number->wordsize;
  new_num->isSign = number->isSign;
  new_num->len = number->len;
  new_num->bits = malloc(sizeof(char)*(number->wordsize+1));
  for (int i = 0; i < number->wordsize; i++) 
    new_num->bits[i] = number->bits[i];
  return new_num;
}

static void calcLength(number_t* number) {
  for (int i = 1; i <= number->wordsize; i++) {
    if (number->bits[number->wordsize-i] == '1')
      number->len = i;
  }
}

/*********************************************/
/*********** OPERATIONS **********************/
/*********************************************/
number_t* add(number_t* a, number_t* b) {
  if (!a || !b) {
    perror("Can't add a NULL number(s)");
    return NULL;
  }
  if (a->wordsize != b->wordsize) {
    perror("Wordsizes are different");
    return NULL;
  }
  number_t* sum = new_number(BINARY, "", a->wordsize);
  char aBit, bBit;
  char carry = '0';
  int ws = a->wordsize;
  for (int i = 1; i <= ws; i++) {
    aBit = a->bits[ws-i];
    bBit = b->bits[ws-i];
    switch (carry+aBit+bBit) {
      case (3*'1'):
        sum->bits[ws-i] = '1';
        carry = '1';
        break;
      case (2*'1'+'0'):
        sum->bits[ws-i] = '0';
        carry = '1';
        break;
      case ('1'+2*'0'):
        sum->bits[ws-i] = '1';
        carry = '0';
        break;
      case (3*'0'):
        sum->bits[ws-i] = '0';
        carry = '0';
        break;
      default:
        perror("something went wrong in full adder");
        return sum;
    }
  }
  calcLength(sum);
  return sum;
}

number_t* lshift(number_t* number, int positions) {
  if (positions < 0) {
    printf("positions must be positive\n");
    return NULL;
  }
  number_t* new_num = copy_number(number);
  char* old = number->bits;
  char* new = new_num->bits;
  int i = 0;
  for (; i < number->wordsize - positions; i++) {
    new[i] = old[i+positions];
  }
  // zero the rest
  for (; i < number->wordsize; i++) 
    new[i] = '0';
  calcLength(new_num);
  return new_num;
}

number_t* rshift(number_t* number, int positions) {
  if (positions < 0) {
    printf("positions must be positive\n");
    return NULL;
  }
  number_t* new_num = copy_number(number);
  char* old = number->bits;
  char* new = new_num->bits;
  int i = 0;
  // fill msb's with zeroes
  for (; i < number->wordsize-number->len+positions; i++)
    new[i] = '0';
  // fill the shifted bits
  for (; i < number->wordsize; i++)
    new[i] = old[i-positions];
  // calculate new length
  calcLength(new_num);  
  return new_num;
}

int isEqual(number_t* a, number_t* b) {
  if (!a || !b) {
    perror("One of the numbers is NULL");
    return 1;
  }
  if (a->len != b->len) {
    printf("A and B's length are different");
    return 1;
  }

  for (int i = 0; i <= a->len; i++) {
    if (a->bits[a->wordsize-i] != b->bits[b->wordsize-i])
      return 1;
  }
  
  // they are equal!
  return 0;
}

/***********************************/
/*********       TESTS     *********/
/***********************************/

#ifdef UNIT_TEST

// int main(int argc, char* argv[]) {
//   printf("Testing Number API\n");
//   number_t* num1;
//   num1 = new_number(BINARY, "111100001111", 8);
//   number_print(num1);
//   delete_number(num1);
//   num1 = new_number(BINARY, "00001111", 8);
//   number_print(num1);
//   delete_number(num1);
//   num1 = new_number(BINARY, "1111", 8);
//   number_print(num1);
//   delete_number(num1);
//   num1 = new_number(BINARY, "01111", 8);
//   number_print(num1);
//   printf("lshift by 2\n");
//   number_t* num2 = lshift(num1, 2);
//   number_print(num2);
//   printf("lshift last by 4\n");
//   number_t* num3 = lshift(num2, 4);
//   number_print(num3);
//   delete_number(num1);
//   delete_number(num2);
//   delete_number(num3);
//   num1 = new_number(BINARY, "11001100", 8);
//   number_print(num1);
//   printf("rshift by 2\n");
//   num2 = rshift(num1, 2);
//   number_print(num2);
//   delete_number(num1);
//   delete_number(num2);
//   num1 = new_number(BINARY, "", 4);
//   number_print(num1);
//   delete_number(num1);
//   num1 = new_number(BINARY, "1101", 8);
//   num2 = new_number(BINARY, "1100", 8);
//   number_print(num1);
//   number_print(num2);
//   printf("adding last two nums\n");
//   num3 = add(num1, num2);
//   number_print(num3);
//   delete_number(num1);
//   delete_number(num2);
//   delete_number(num3);
// }

int isEqualToBitstring (number_t* n, char* s) {
  if (!n || !s) {
    printf("%d\n", __LINE__);
    return 1;
  }
  int len = strlen(s);
  // check if they are both zero
  if (len == 1 && s[0] == '0' && n->len == 0)
    return 0;
  if (len != n->len) {
    printf("%d\n", __LINE__);
    return 1;
  }
  for (int i = 1; i <= n->len; i++) {
    if (n->bits[n->wordsize-i] != s[len-i]) {
      printf("%d\n", __LINE__);
      return 1;
    }
  }
  return 0;
}

int test_add (char* aS, char* bS, char* expected, int wordsize, char*  msg) {
  printf("_____ ADD a+b (%d-bit Numbers) _____\n", wordsize);
  if (msg != NULL) 
    printf("Objective: %s\n", msg);
  number_t* a = new_number(BINARY, aS, wordsize);
  number_t* b = new_number(BINARY, bS, wordsize);
  printf("a = "); printBits(a); printf("\n");
  printf("b = "); printBits(b); printf("\n");
  printf("expected a+b = %s\n", expected);
  number_t* sum = add(a, b);
  printf("actual a+b = "); printBits(sum); printf("\n");
  int ret = isEqualToBitstring(sum, expected);
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed!\n");
  delete_number(a);
  delete_number(b);
  delete_number(sum);
  return ret;
}

#endif
