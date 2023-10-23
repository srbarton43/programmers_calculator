
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/number.h"

typedef struct number {
  char binary[65];
  long decimal_signed;
  unsigned long decimal_unsigned;
  char hex[17];
} number_t;


static unsigned long binary2dec(char* binary);
static void binary2hex(char* hex, char* binary);
static void dec2binary(unsigned long decimal, char* binary);
static void hex2binary(char* binary, char* hex);

number_t* new_number(int type, char* number) {
  if (number == NULL) {
    fprintf(stderr, "new_number: null number string");
    return NULL;
  }
  number_t* new_num = malloc(sizeof(number_t));
  if (type == BINARY) {
    sprintf(new_num->binary, "%s", number);
    new_num->decimal_unsigned = binary2dec(number);
    new_num->decimal_signed = (long) new_num->decimal_unsigned;
    binary2hex(new_num->hex, number);
  } else if (type == DECIMAL) {
    new_num->decimal_signed = atoi(number);
    new_num->decimal_unsigned = (unsigned long) new_num->decimal_signed;
    dec2binary(new_num->decimal_unsigned, new_num->binary);
    binary2hex(new_num->hex, new_num->binary);
  } else if (type == HEX) {
    sprintf(new_num->hex, "%s", number);
    hex2binary(new_num->binary, new_num->hex);
    new_num->decimal_unsigned = binary2dec(new_num->binary);
    new_num->decimal_signed = (signed long) new_num->decimal_unsigned;
  } else {
    fprintf(stderr, "new_number: %d is not a valid number type", type);
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

/********* binary2dec *************/
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
static void binary2hex (char* hex, char* binary) {
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
      return;
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
  strcpy(hex, hex_arr);
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
  }
  int bitLength = strlen(hex)*4 + partial + 1;
  char bitstring[bitLength];
  bitstring[bitLength-1] = '\0';
  int bitPointer = bitLength - 2;
  int hexPointer = strlen(hex) - 1;
  while (hexPointer >= 0) {
    int nibbleVal;
    char nibble = hex[hexPointer];
    if (nibble > 'f' || nibble < 'a' && !(nibble < '0' || nibble > '9')) {
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
  free(number);
}

void number_print(number_t* number) {
  printf("NUMBER %p\n", number);
  printf("BINARY: %s\n", number->binary);
  printf("UNSIGNED DECIMAL: %lu\n", number->decimal_unsigned);
  printf("SIGNED DECIMAL: %ld\n", number->decimal_signed);
  printf("HEX: 0x%s\n", number->hex);
}
