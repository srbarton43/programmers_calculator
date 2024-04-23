// A THOUGHT
// operations could store result in second param? like asm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "number.h"
#include "utils.h"

number_t *_zero_;
number_t *_one_;

static void calcLength(number_t *number);
static void printBits(number_t *num);

number_t *new_number(type_e type, const char *number, int wordsize) {
  if (number == NULL) {
    perror("null number string");
    return NULL;
  }
  number_t *new_num = malloc(sizeof(number_t));
  new_num->wordsize = wordsize;
  new_num->len = 0;
  new_num->bits = malloc(wordsize * sizeof(char));
  // zero bits
  for (int i = 0; i < wordsize; i++) {
    new_num->bits[i] = '0';
  }

  int slen = strlen(number);
  if (slen == 1 && number[0] == '0') {
    // the number is zero
  } else {
    char bits[65];
    switch (type) {
    case BINARY: {
      int i;
      for (i = 1; i <= wordsize; i++) {
        if (i <= slen)
          new_num->bits[wordsize - i] = number[slen - i];
        else
          new_num->bits[wordsize - i] = '0';
      }
      if (i <= slen)
        printf("binary bigger than wordsize\n");

      break;
    }
    case DECIMAL: {
      unsigned long decimal = atol(number);
      dec2binary(decimal, bits, wordsize);
      slen = strlen(bits);
#ifdef DEBUG
      printf("slen: %d\n", slen);
#endif
      int i;
      for (i = 1; i <= wordsize; i++) {
        if (i <= slen)
          new_num->bits[wordsize - i] = bits[slen - i];
        else
          new_num->bits[wordsize - i] = '0';
      }
      if (i <= slen)
        printf("decimal too big\n");
      break;
    }
    case HEXADECIMAL: {
      char bits[65];
      hex2binary(number, bits, wordsize);
#ifdef DEBUG
      printf("hex number: %s\n", bits);
#endif
      slen = strlen(bits);
      for (int i = 1; i <= wordsize; i++) {
        if (i <= slen)
          new_num->bits[wordsize - i] = bits[slen - i];
        else
          new_num->bits[wordsize - i] = '0';
      }
      break;
    }
    default:
      perror("new_number: not a supported number type");
      free(new_num);
      return NULL;
    }
  }
  int i = wordsize - 1;
  while (i >= 0) {
    if (new_num->bits[i] == '1')
      new_num->len = wordsize - i;
    i--;
  }
  return new_num;
}

void change_wordsize(number_t *num, int wordsize) {
  int old_ws = num->wordsize;
  char *old_bits = num->bits;
  char *bits = malloc(wordsize * sizeof(char));
  for (int i = 1; i <= wordsize; i++) {
    if (old_ws - i >= 0)
      bits[wordsize - i] = old_bits[old_ws - i];
    else
      bits[wordsize - i] = '0';
  }
  free(old_bits);
  num->bits = bits;
  num->wordsize = wordsize;
  calcLength(num);
}

/********* number_getUdec *************/
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
unsigned long long number_getUdec(number_t *num) {
  if (num == NULL) {
    fprintf(stderr, "number_getUdec: null number\n");
    return 0;
  }
  char *bin = num->bits;
  unsigned long long value = 0;
  for (int i = 1; i <= num->len; i++) {
    char bit = bin[num->wordsize - i];
#ifdef DEBUG
    printf("bit: %c\n", bit);
#endif
    if (bit == '1') {
      value += (long long)1 << (i - 1);
    }
  }
  return value;
}

/********* number_getSdec *************/
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
signed long long number_getSdec(number_t *num) {
  if (num == NULL) {
    fprintf(stderr, "number_getSdec: null number\n");
    return 0;
  }
  char *bin = num->bits;
  long long value;
  if (bin[0] == '1')
    value = -((long long)1 << (num->wordsize - 1));
  else
    value = 0;
  for (int i = 1; i <= num->len && i < num->wordsize; i++) {
    char bit = bin[num->wordsize - i];
#ifdef DEBUG
    printf("bit: %c\n", bit);
#endif
    if (bit == '1') {
      value += (long long)1 << (i - 1);
    }
  }
  return value;
}

char *number_getHex(number_t *number) {
  if (numbers_are_equal(_zero_, number)) {
    char *hex = malloc(sizeof("0x0\0"));
    strcpy(hex, "0x0");
    return hex;
  }
  int hLen;
  int nLen = number->len;
  if (nLen % 4 == 0)
    hLen = nLen / 4 + 3;
  else
    hLen = nLen / 4 + 4;
  char *hex = malloc((hLen) * sizeof(char));
  int nibble;
  int hp = 2;
  int i = 1;
  while (i <= nLen) {
    nibble = 0;
    int j;
    for (j = i; j < i + 4 && j <= nLen; j++) {
      nibble += ((number->bits[number->wordsize - j] - '0') << (j - i));
    }
    i = j;
    if (nibble < 10)
      hex[hLen - hp] = nibble + '0';
    else
      hex[hLen - hp] = nibble + 'a' - 10;
    hp++;
  }
  hex[0] = '0';
  hex[1] = 'x';
  hex[hLen - 1] = 0;
  return hex;
}

int dec2binary(unsigned long long decimal, char *binary, int wordsize) {
  if (binary == NULL) {
    fprintf(stderr, "dec2binary: bitstring");
    return ERROR;
  }
  char bits_reversed[wordsize + 1];
  int pointer = 0;
  while (decimal != 0) {
    if (pointer >= wordsize) {
      return ERROR;
    }
    int rem = decimal % 2;
    bits_reversed[pointer] = rem + '0';
    pointer++;
    decimal = decimal >> 1;
  }
  char bitstring[wordsize + 1];
  int i = wordsize - 1;
  int p = 0;
  while (i >= 0) {
    if (p < pointer)
      bitstring[i] = bits_reversed[p];
    else
      bitstring[i] = '0';
    i--;
    p++;
  }
  bitstring[wordsize] = '\0';
  strcpy(binary, bitstring);
  return SUCCESS;
}

int hex2binary(const char *hex, char *binary, int wordsize) {
  if (hex == NULL) {
    printf("hex2binary: null hexstring passed\n");
    return ERROR;
  }
  char bitstring[wordsize + 1];
  bitstring[wordsize] = 0;
  int bitPointer = wordsize - 1;
  int hexPointer = strlen(hex) - 1;
  while (hexPointer >= 0) {
    int nibbleVal;
    char nibble = hex[hexPointer--];
    if (nibble <= 'f' && nibble >= 'a') { // valid letter
      nibbleVal = nibble - 'a' + 10;
    } else if (nibble >= '0' && nibble <= '9') { // valid number
      nibbleVal = nibble - '0';
    } else {
      printf("hex2binary: invalid hex char\n");
      return ERROR;
    }
    for (int i = 0; i < 4; i++) {
      if (hexPointer < 0 && nibbleVal == 0) {
#ifdef DEBUG
        printf("hit MSB of bit string on MSN\n");
#endif
        break;
      }
      if (bitPointer < 0) { // number is huge
#ifdef DEBUG
        printf("number is too big\n");
#endif
        return ERROR;
      }
      int rem = nibbleVal % 2;
      bitstring[bitPointer--] = rem + '0';
      nibbleVal = nibbleVal >> 1;
    }
  }
  while (bitPointer >= 0)
    bitstring[bitPointer--] = '0';
  // remove leading zeroes
  // char* p = bitstring;
  // while (*p != 0 && *p == '0') p++;
  strcpy(binary, bitstring);
  return SUCCESS;
}

void delete_number(number_t *number) {
  if (number) {
    free(number->bits);
    free(number);
  }
}

void init_numbers(void) {
  _zero_ = new_number(BINARY, "0", 1);
  _one_ = new_number(BINARY, "1", 2);
}

void free_numbers(void) {
  delete_number(_zero_);
  delete_number(_one_);
}

void number_print(number_t *number) {
  printf("--------------\n");
#ifdef DEBUG
  printf("NUMBER %p\n", number);
#endif
  printf("WORDSIZE %d\n", number->wordsize);
#ifdef DEBUG
  printf("LENGTH %d\n", number->len);
#endif
  // printf("BITSTRING "); printBits(number); printf("\n");
  char *bs = number->bits;
  printf("BITSTRING ");
  for (int i = 0; i < number->wordsize; i++)
    printf("%c", bs[i]);
  printf("\n");
  printf("Integer Value: %lld\n", number_getSdec(number));
  printf("Unsigned Integer Value: %llu\n", number_getUdec(number));
  char *hex = number_getHex(number);
  printf("Hexadecimal Value: %s\n", hex);
  free(hex);
  printf("--------------\n");
}

static void printBits(number_t *num) {
  char *bs = num->bits;
  if (num->len == 0) {
    printf("0");
  } else {
    for (int i = num->wordsize - num->len; i < num->wordsize; i++) {
      printf("%c", bs[i]);
    }
  }
}

number_t *copy_number(number_t *number, int wordsize) {
  number_t *new_num = malloc(sizeof(number_t));
  if (wordsize == 0) {
    new_num->wordsize = number->wordsize;
    printf("don't do this\n");
    exit(69);
  } else
    new_num->wordsize = wordsize;
  new_num->len = min(number->len, new_num->wordsize);
  new_num->bits = malloc(sizeof(char) * (new_num->wordsize + 1));
  for (int i = 1; i <= new_num->wordsize; i++)
    if (i <= number->wordsize)
      new_num->bits[new_num->wordsize - i] = number->bits[number->wordsize - i];
    else
      new_num->bits[new_num->wordsize - i] = '0';
  return new_num;
}

static void calcLength(number_t *number) {
  number->len = 0;
  for (int i = 1; i <= number->wordsize; i++) {
    if (number->bits[number->wordsize - i] == '1')
      number->len = i;
  }
}

/*********************************************/
/*********** OPERATIONS **********************/
/*********************************************/

number_t *ones_comp(number_t *num, int wordsize) {
  number_t *ones;
  if (wordsize == 0)
    ones = copy_number(num, 0);
  else
    ones = copy_number(num, wordsize);

  // flip bits
  for (int i = 1; i <= ones->wordsize; i++) {
    if (ones->bits[ones->wordsize - i] == '1')
      ones->bits[ones->wordsize - i] = '0';
    else
      ones->bits[ones->wordsize - i] = '1';
  }
  calcLength(ones);
  return ones;
}
number_t *twos_comp(number_t *num, int wordsize) {
  number_t *ones;

  ones = ones_comp(num, wordsize);

  // add one
  number_t *twos = add(_one_, ones, wordsize);

  // cleanup
  delete_number(ones);

  return twos;
}

number_t *add(number_t *a, number_t *b, int wordsize) {
  if (!a || !b) {
    perror("Can't add a NULL number(s)");
    return NULL;
  }
#ifdef DEBUG
  printf("asize: %d; bsize: %d, prog_data->ws: %d\n", a->wordsize, b->wordsize,
         wordsize);
#endif
  number_t *sum = new_number(BINARY, "", wordsize);
  char aBit, bBit;
  char carry = '0';
  for (int i = 1; i <= wordsize; i++) {
    if (i <= a->wordsize)
      aBit = a->bits[a->wordsize - i];
    else
      aBit = '0';
    if (i <= b->wordsize)
      bBit = b->bits[b->wordsize - i];
    else
      bBit = '0';

#ifdef DEBUG
    printf("aBit: %c; bBit: %c; i: %d\n", aBit, bBit, i);
#endif
    switch (carry + aBit + bBit) {
    case (3 * '1'):
      sum->bits[wordsize - i] = '1';
      carry = '1';
      break;
    case (2 * '1' + '0'):
      sum->bits[wordsize - i] = '0';
      carry = '1';
      break;
    case ('1' + 2 * '0'):
      sum->bits[wordsize - i] = '1';
      carry = '0';
      break;
    case (3 * '0'):
      sum->bits[wordsize - i] = '0';
      carry = '0';
      break;
    default:
      perror("something went wrong in full adder");
      return sum;
    }
  }
  calcLength(sum);
  global_nums_flag.sign = sum->bits[0] == '1';
  global_nums_flag.overflow =
      (sum->bits[0] == '1' && a->bits[0] == '0' && b->bits[0] == '0') ||
      (sum->bits[0] == '0' && a->bits[0] == '1' && b->bits[0] == '1');
  return sum;
}

number_t *sub(number_t *a, number_t *b, int wordsize) {
  number_t *neg_a = twos_comp(a, max(b->wordsize, a->wordsize));
  number_t *sum = add(b, neg_a, wordsize);

  delete_number(neg_a);

  return sum;
}

number_t *lshift(number_t *number, number_t *positions, int wordsize) {
  int pos = number_getUdec(positions);
  if (pos < 0) {
    printf("pos must be positive\n");
    return NULL;
  }
  number_t *new_num = copy_number(number, wordsize);
  char *old = number->bits;
  char *new = new_num->bits;
  int i = 0;
  for (; i < number->wordsize - pos; i++) {
    new[i] = old[i + pos];
  }
  // zero the rest
  for (; i < number->wordsize; i++)
    new[i] = '0';
  calcLength(new_num);
  return new_num;
}

number_t *rshift(number_t *number, number_t *positions, int wordsize) {
  int pos = number_getUdec(positions);
  if (pos < 0) {
    printf("pos must be positive\n");
    return NULL;
  }
  number_t *new_num = copy_number(number, wordsize);
  char *old = number->bits;
  char *new = new_num->bits;
  int i = 0;
#ifdef DEBUG
  printf("len: %d, ws: %d\n", new_num->len, new_num->wordsize);
#endif
  // fill msb's with zeroes
  for (; i < number->wordsize && i < number->wordsize - number->len + pos;
       i++) {
#ifdef DEBUG
    printf("idx: %d\n", i);
#endif
    new[i] = '0';
  }
  // fill the shifted bits
  for (; i < number->wordsize; i++)
    new[i] = old[i - pos];
  // calculate new length
  calcLength(new_num);
  return new_num;
}

/*           and             */
number_t * and (number_t * a, number_t *b, int wordsize) {
  number_t *num = copy_number(_zero_, wordsize);
  int aWs = a->wordsize;
  int bWs = b->wordsize;
  for (int i = 1; i <= wordsize; i++) {
    num->bits[wordsize - i] =
        (a->bits[aWs - i] == '1' && b->bits[bWs - i] == '1') + '0';
  }
  calcLength(num);
  return num;
}

/*             or              */
number_t * or (number_t * a, number_t *b, int wordsize) {
  number_t *num = copy_number(_zero_, wordsize);
  int aWs = a->wordsize;
  int bWs = b->wordsize;
  for (int i = 1; i <= wordsize; i++) {
    num->bits[wordsize - i] =
        (a->bits[aWs - i] == '1' || b->bits[bWs - i] == '1') + '0';
  }
  calcLength(num);
  return num;
}

int numbers_are_equal(number_t *a, number_t *b) {
  if (!a || !b) {
    perror("One of the numbers is NULL");
    return 0;
  }
  if (a->len != b->len) {
#ifdef DEBUG
    printf("A and B's length are different\n");
#endif
    return 0;
  }

  for (int i = 0; i <= a->len; i++) {
    if (a->bits[a->wordsize - i] != b->bits[b->wordsize - i])
      return 0;
  }

  // they are equal!
  return 1;
}

/***********************************/
/*********       TESTS     *********/
/***********************************/

#ifdef UNIT_TEST

int isEqualToBitstring(number_t *n, char *s) {
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
    if (n->bits[n->wordsize - i] != s[len - i]) {
      printf("%d\n", __LINE__);
      return 1;
    }
  }
  return 0;
}

int test_twos_comp(char *num, char *expected, int wordsize, char *msg) {
  printf("_____ TWO's COMPLEMENT(num) with %d-bit words _____\n", wordsize);
  if (msg != NULL)
    printf("Objective: %s\n", msg);
  number_t *n = new_number(BINARY, num, wordsize);
  printf("num = ");
  printBits(n);
  printf("\n");
  printf("expected two's complement = %s\n", expected);
  number_t *twos = twos_comp(n, wordsize);
  printf("actual two's complement = ");
  printBits(twos);
  printf("\n");
  int ret = isEqualToBitstring(twos, expected);
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed!\n");
  delete_number(n);
  delete_number(twos);
  return ret;
}

int test_lshift(char *num, char *pos, char *expected, int wordsize, char *msg) {
  printf("_____ LSHIFT num << pos (%d-bit Numbers) _____\n", wordsize);
  if (msg != NULL)
    printf("Objective: %s\n", msg);
  number_t *n = new_number(BINARY, num, wordsize);
  number_t *p = new_number(BINARY, pos, wordsize);
  printf("num = ");
  printBits(n);
  printf("\n");
  printf("pos = ");
  printBits(p);
  printf("\n");
  printf("expected num << pos = %s\n", expected);
  number_t *res = lshift(n, p, wordsize);
  printf("actual num << pos = ");
  printBits(res);
  printf("\n");
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

int test_rshift(char *num, char *pos, char *expected, int wordsize, char *msg) {
  printf("_____ RSHIFT num >> pos (%d-bit Numbers) _____\n", wordsize);
  if (msg != NULL)
    printf("Objective: %s\n", msg);
  number_t *n = new_number(BINARY, num, wordsize);
  number_t *p = new_number(BINARY, pos, wordsize);
  printf("num = ");
  printBits(n);
  printf("\n");
  printf("pos = ");
  printBits(p);
  printf("\n");
  printf("expected num >> pos = %s\n", expected);
  number_t *res = rshift(n, p, wordsize);
  printf("actual num >> pos = ");
  printBits(res);
  printf("\n");
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

int test_add(char *aS, int aWs, char *bS, int bWs, char *expected, char *msg) {
  printf("_____ ADD a+b (%d-bit + %d-bit) _____\n", aWs, bWs);
  if (msg != NULL)
    printf("Objective: %s\n", msg);
  number_t *a = new_number(BINARY, aS, aWs);
  number_t *b = new_number(BINARY, bS, bWs);
  printf("a = ");
  printBits(a);
  printf("\n");
  printf("b = ");
  printBits(b);
  printf("\n");
  printf("expected a+b = %s\n", expected);
  number_t *sum = add(a, b, max(aWs, bWs));
  printf("actual a+b = ");
  printBits(sum);
  printf("\n");
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

int test_copy_number(char *num, int iws, int ows, char *expected, char *msg) {
  printf("_____ COPY num (%d-bit -> %d-bit) _____\n", iws, ows);
  if (msg != NULL)
    printf("Objective: %s\n", msg);
  number_t *n = new_number(BINARY, num, iws);
  printf("num = ");
  printBits(n);
  printf("\n");
  printf("expected new_num = %s\n", expected);
  number_t *new_n = copy_number(n, ows);
  printf("actual new_num = ");
  printBits(new_n);
  printf("\n");
  int ret = isEqualToBitstring(new_n, expected);
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed :(\n");
  return ret;
  delete_number(n);
  delete_number(new_n);
}

int test_sub(char *aS, int aWs, char *bS, int bWs, char *expected, char *msg) {
  printf("_____ SUB b-a (%d-bit - %d-bit) _____\n", bWs, aWs);
  if (msg != NULL)
    printf("Objective: %s\n", msg);
  number_t *a = new_number(BINARY, aS, aWs);
  number_t *b = new_number(BINARY, bS, bWs);
  printf("b = ");
  printBits(b);
  printf("\n");
  printf("a = ");
  printBits(a);
  printf("\n");
  printf("expected b-a = %s\n", expected);
  number_t *sum = sub(a, b, max(aWs, bWs));
  printf("actual b-a = ");
  printBits(sum);
  printf("\n");
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

int test_and(int ws, char *aS, int aWs, char *bS, int bWs, char *expected,
             char *msg) {
  printf("_____ AND a&b (%d-bit - %d-bit) _____\n", aWs, bWs);
  if (msg != NULL)
    printf("Objective: %s\n", msg);
  number_t *a = new_number(BINARY, aS, aWs);
  number_t *b = new_number(BINARY, bS, bWs);
  printf("b = ");
  printBits(b);
  printf("\n");
  printf("a = ");
  printBits(a);
  printf("\n");
  printf("expected a&b = %s\n", expected);
  number_t *anded = and(a, b, ws);
  number_print(anded);
  printf("actual a&b = ");
  printBits(anded);
  printf("\n");
  int ret = isEqualToBitstring(anded, expected);
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed!\n");
  delete_number(a);
  delete_number(b);
  delete_number(anded);
  return ret;
}
int test_or(int ws, char *aS, int aWs, char *bS, int bWs, char *expected,
            char *msg) {
  printf("_____ OR a|b (%d-bit | %d-bit) _____\n", aWs, bWs);
  if (msg != NULL)
    printf("Objective: %s\n", msg);
  number_t *a = new_number(BINARY, aS, aWs);
  number_t *b = new_number(BINARY, bS, bWs);
  printf("a = ");
  printBits(a);
  printf("\n");
  printf("b = ");
  printBits(b);
  printf("\n");
  printf("expected a|b = %s\n", expected);
  number_t *ored = or (a, b, ws);
  printf("actual a|b = ");
  printBits(ored);
  printf("\n");
  int ret = isEqualToBitstring(ored, expected);
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed!\n");
  delete_number(a);
  delete_number(b);
  delete_number(ored);
  return ret;
}

int test_dec2binary(unsigned long long decimal, char *expected, int wordsize) {
  printf("decimal = %llu\n", decimal);
  printf("expected bitstring = %s\n", expected);
  char out[65];
  dec2binary(decimal, out, wordsize);
  printf("actual bitstring =   %s\n", out);
  int ret = strcmp(expected, out) != 0;
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed!\n");
  return ret;
}

int test_hex2binary(char *hex, char *expected) {
  printf("hexstring = 0x%s\n", hex);
  printf("expected bitstring = %s\n", expected);
  char out[65];
  hex2binary(hex, out, strlen(expected));
  printf("actual bitstring =   %s\n", out);
  int ret = strcmp(expected, out) != 0;
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed!\n");
  return ret;
}
#endif
