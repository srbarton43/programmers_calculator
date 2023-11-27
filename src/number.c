// A THOUGHT
// operations could store result in second param? like asm

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/number.h"
#include "../include/utils.h"

number_t* _zero_;
number_t* _one_;

static unsigned int binary2udec(number_t* number);
static char* binary2hex(char* binary);
static void hex2binary(char* binary, char* hex);
static void calcLength(number_t* number);
static void printBits(number_t* num);

number_t* new_number(type_e type, const char* number, int wordsize) {
  if (number == NULL) {
    perror("null number string");
    return NULL;
  }
  number_t* new_num = malloc(sizeof(number_t));
  new_num->wordsize = wordsize;
  new_num->len = 0;
  new_num->bits = malloc(wordsize*sizeof(char));
  // zero bits
  for (int i = 0; i < wordsize; i++) {
    new_num->bits[i] = '0';
  }

  unsigned long decimal;

  int slen = strlen(number);
  if (slen == 1 && number[0] == '0') {
    // the number is zero
  } else {
  switch (type) {
    case BINARY:
      for (int i = 1; i <= wordsize; i++) {
        if (i <= slen)
          new_num->bits[wordsize-i] = number[slen-i];
        else
          new_num->bits[wordsize-i] = '0';
      }
      
      break;
    case DECIMAL:
      decimal = atol(number);
      char bits[65];
      dec2binary(decimal, bits);
      slen = strlen(bits);
      for (int i = 1; i <= wordsize; i++) {
        if (i <= slen)
          new_num->bits[wordsize-i] = bits[slen-i];
        else
          new_num->bits[wordsize-i] = '0';
      }
      break;

    default:
      perror("new_number: not a supported number type");
      free(new_num);
      return NULL;
    }
  }
  int i = wordsize-1;
  while (i >= 0 ) {
    if (new_num->bits[i] == '1')
      new_num->len = wordsize - i;
    i--;
  }
  return new_num;
}

/********* binary2udec *************/
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
static unsigned int binary2udec(number_t* num) {
  if (num == NULL) {
    fprintf(stderr, "binary2udec: null number\n");
    return 0;
  }
  char* bin = num->bits;
  unsigned long value = 0;
  for (int i = 1; i <= num->len; i++) {
    char bit = bin[num->wordsize - i];
#ifdef DEBUG
    printf("bit: %c\n", bit);
#endif
    if (bit == '1') {
      value += 1 << (i-1);
    }
  }
  return value;
}

/********* binary2sdec *************/
/* calculates the signed decimal for bitstring
 *
 * parameters:
 *  char* binary => bitstring
 * returns:
 *  int => unsigned decimal given by bitstring
 *
 * We Assume:
 *  binary contains only 1's and 0's
 */
static unsigned int binary2sdec(number_t* num) {
  if (num == NULL) {
    fprintf(stderr, "binary2sdec: null number\n");
    return 0;
  }
  char* bin = num->bits;
  long value;
  if (bin[0] == '1')
    value = - (1 << (num->wordsize-1));
  else
    value = 0;
  for (int i = 1; i <= num->len && i < num->wordsize; i++) {
    char bit = bin[num->wordsize - i];
#ifdef DEBUG
    printf("bit: %c\n", bit);
#endif
    if (bit == '1') {
      value += 1 << (i-1);
    }
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
    fprintf(stderr, "binary2udec: null bitstring\n");
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
  if (number) {
    free(number->bits);
    free(number);
  }
}

void init_numbers(void) {
  _zero_ = new_number(BINARY, "0", WORDSIZE);
  _one_ = new_number(BINARY, "1", WORDSIZE);
}

void free_numbers(void) {
  delete_number(_zero_);
  delete_number(_one_);
}

void number_print(number_t* number) {
  printf("--------------\n");
  printf("NUMBER %p\n", number);
  printf("WORDSIZE %d\n", number->wordsize);
  printf("LENGTH %d\n", number->len);
  char* bs = number->bits;
  printf("BITSTRING "); printBits(number); printf("\n");
  printf("RAW ");
  for (int i = 0; i < number->wordsize; i++)
    printf("%c", bs[i]);
  printf("\n");
  printf("Unsigned Decimal Value: %d\n", binary2udec(number));
  printf("Signed Decimal Value: %d\n", binary2sdec(number));
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

number_t* copy_number(number_t* number) {
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
  number->len = 0;
  for (int i = 1; i <= number->wordsize; i++) {
    if (number->bits[number->wordsize-i] == '1')
      number->len = i;
  }
}

/*********************************************/
/*********** OPERATIONS **********************/
/*********************************************/

number_t* twos_comp(number_t* num) {
  number_t* ones = copy_number(num);
  
  // flip bits
  for (int i = 0; i < num->wordsize; i++) {
    if (num->bits[i] == '1') 
      ones->bits[i] = '0';
    else
      ones->bits[i] = '1';
  }
  
  // add one
  number_t* one = new_number(BINARY, "1", num->wordsize);
  
  number_t* twos = add(one, ones);
  
  // cleanup
  delete_number(ones);
  delete_number(one);
  
  return twos;
}

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

number_t* sub (number_t* a, number_t* b) {
  number_t* neg_a = twos_comp(a);
  number_t* sum = add(b, neg_a);

  delete_number(neg_a);

  return sum;
}

number_t* lshift(number_t* number, number_t* positions) {
  int pos = binary2udec(positions);
  if (pos < 0) {
    printf("pos must be positive\n");
    return NULL;
  }
  number_t* new_num = copy_number(number);
  char* old = number->bits;
  char* new = new_num->bits;
  int i = 0;
  for (; i < number->wordsize - pos; i++) {
    new[i] = old[i+pos];
  }
  // zero the rest
  for (; i < number->wordsize; i++) 
    new[i] = '0';
  calcLength(new_num);
  return new_num;
}

number_t* rshift(number_t* number, number_t* positions) {
  int pos = binary2udec(positions);
  if (pos < 0) {
    printf("pos must be positive\n");
    return NULL;
  }
  number_t* new_num = copy_number(number);
  char* old = number->bits;
  char* new = new_num->bits;
  int i = 0;
#ifdef DEBUG
    printf("len: %d, ws: %d\n", new_num->len, new_num->wordsize);
#endif
  // fill msb's with zeroes
  for (; i < number->wordsize && i < number->wordsize - number->len + pos; i++) {
#ifdef DEBUG
    printf("idx: %d\n", i);
#endif
    new[i] = '0';
  }
  // fill the shifted bits
  for (; i < number->wordsize; i++)
    new[i] = old[i-pos];
  // calculate new length
  calcLength(new_num);  
  return new_num;
}

int numbers_are_equal(number_t* a, number_t* b) {
  if (!a || !b) {
    perror("One of the numbers is NULL");
    return 0;
  }
  if (a->len != b->len) {
    printf("A and B's length are different");
    return 0;
  }

  for (int i = 0; i <= a->len; i++) {
    if (a->bits[a->wordsize-i] != b->bits[b->wordsize-i])
      return 0;
  }
  
  // they are equal!
  return 1;
}

/***********************************/
/*********       TESTS     *********/
/***********************************/

#ifdef UNIT_TEST

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

int test_twos_comp(char* num, char* expected, int wordsize, char* msg) {
  printf("_____ TWO's COMPLEMENT(num) with %d-bit words _____\n", wordsize);
  if (msg != NULL) 
    printf("Objective: %s\n", msg);
  number_t* n = new_number(BINARY, num, wordsize);
  printf("num = "); printBits(n); printf("\n");
   printf("expected two's complement = %s\n", expected);
  number_t* twos = twos_comp(n);
  printf("actual two's complement = "); printBits(twos); printf("\n");
  int ret = isEqualToBitstring(twos, expected);
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Passed!\n");
  delete_number(n);
  delete_number(twos);
  return ret;
}

int test_lshift (char* num, char* pos, char* expected, int wordsize, char* msg) {
  printf("_____ LSHIFT num << pos (%d-bit Numbers) _____\n", wordsize);
  if (msg != NULL) 
    printf("Objective: %s\n", msg);
  number_t* n = new_number(BINARY, num, wordsize);
  number_t* p = new_number(BINARY, pos, wordsize);
  printf("num = "); printBits(n); printf("\n");
  printf("pos = "); printBits(p); printf("\n");
  printf("expected num << pos = %s\n", expected);
  number_t* res = lshift(n, p);
  printf("actual num << pos = "); printBits(res); printf("\n");
  int ret = isEqualToBitstring(res, expected);
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed!\n");
  delete_number(n);
  delete_number(p);
  delete_number(res);
  return ret;
}

int test_rshift (char* num, char* pos, char* expected, int wordsize, char* msg) {
  printf("_____ RSHIFT num >> pos (%d-bit Numbers) _____\n", wordsize);
  if (msg != NULL) 
    printf("Objective: %s\n", msg);
  number_t* n = new_number(BINARY, num, wordsize);
  number_t* p = new_number(BINARY, pos, wordsize);
  printf("num = "); printBits(n); printf("\n");
  printf("pos = "); printBits(p); printf("\n");
  printf("expected num >> pos = %s\n", expected);
  number_t* res = rshift(n, p);
  printf("actual num >> pos = "); printBits(res); printf("\n");
  int ret = isEqualToBitstring(res, expected);
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed!\n");
  delete_number(n);
  delete_number(p);
  delete_number(res);
  return ret;
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

int test_sub (char* aS, char* bS, char* expected, int wordsize, char*  msg) {
  printf("_____ SUB b-a (%d-bit Numbers) _____\n", wordsize);
  if (msg != NULL) 
    printf("Objective: %s\n", msg);
  number_t* a = new_number(BINARY, aS, wordsize);
  number_t* b = new_number(BINARY, bS, wordsize);
  printf("b = "); printBits(b); printf("\n");
  printf("a = "); printBits(a); printf("\n");
  printf("expected b-a = %s\n", expected);
  number_t* sum = sub(a, b);
  printf("actual b-a = "); printBits(sum); printf("\n");
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
