#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "number.h"

#define MASK get_max_unsigned(wordsize)
// size for num rep. arr.
#define WIDTH 64
#define SIZE_BYTES SIZE*WIDTH/8
#define ZERO(ws) {ws, {0}, {0}}

// bitfield for number metadata
struct md_bf {
  unsigned int UNSIGNED_OVERFLOW  : 1;
  unsigned int SIGNED_OVERFLOW    : 1;
};

typedef struct number {
  int wordsize;           // wordsize for the bitstring
  u64 num[SIZE];             // stores bitstring (only conisider [wordsize] LSB's
  struct md_bf metadata;  // stores number metadata about overflow, etc
} number_t;

number_t _zero_ = {1, {0}, {0, 0}};
number_t _one_ = {2, {0,1}, {0, 0}};

static void print_u64(u64 *num, int wordsize);
static void print_hex(u64 *num, int wordsize);
static int bitstring_to_u64(const char *bitstring, int wordsize, u64 *out);
static int hexstring_to_u64(const char *hexstring, int wordsize, u64 *out);
static int decstring_to_u64(const char *decstring, int wordsize, u64 *out);
static int bubble_up_overflows(number_t *out, number_t *a, number_t *b);
static u64 get_nibble_val(char c);
static u64 get_max_unsigned(int wordsize);
static u64 get_max_number(number_t *out, int wordsize);

int new_number(number_t *out, type_e type, const char *number, int wordsize) {
  if (number == NULL) {
    perror("null number string");
    return ERROR;
  }
#ifdef DEBUG
  printf("%s: type=%d, number=%s, wordsize=%d\n", __FUNCTION__, type, number, wordsize);
#endif
  number_t *new_num = out;
  memset(new_num, 0, sizeof(number_t));
  new_num->wordsize = wordsize;
  int ret = SUCCESS;

  int slen = strlen(number);
  if (slen == 1 && number[0] == '0') {
    // the number is zero
  } else {
    // check return codes here
    switch (type) {
    case BINARY: {
      if (ERROR == bitstring_to_u64(number, wordsize, new_num->num))
        new_num->metadata.UNSIGNED_OVERFLOW = 1;
      break;
    }
    case DECIMAL: {
      // TODO
      printf("Unsupported currently.\n");
      u64 raw_decimal[SIZE] = {0};
      break;
      if (ERROR == decstring_to_u64(number, wordsize, raw_decimal))
          new_num->metadata.UNSIGNED_OVERFLOW = 1;
      //if ((wordsize == 64 && raw_decimal > UINT64_MAX) ||
      //    (wordsize < 64 && raw_decimal > (1ULL << wordsize) - 1ULL))
      //  new_num->metadata.UNSIGNED_OVERFLOW = 1;
      //else if (raw_decimal & 1ULL << (wordsize - 1))
      //  new_num->metadata.SIGNED_OVERFLOW = 1;
      memcpy(new_num->num, raw_decimal, SIZE_BYTES);
      // new_num->num = raw_decimal & MASK;
      break;
    }
    case HEXADECIMAL: {
      if (ERROR == hexstring_to_u64(number, wordsize, new_num->num))
        new_num->metadata.UNSIGNED_OVERFLOW = 1;
      break;
    }
    default:
      perror("new_number: not a supported number type");
      free(new_num);
      return ERROR;
    }
  }
  return ret;
}

static int bitstring_to_u64(const char *bitstring, int wordsize, u64 *out) {
  u64 num[SIZE] = {0};
  int slen = strlen(bitstring);

  int i;
  for (i = 1; i <= wordsize; i++) {
    if (i <= slen) {
      num[SIZE - (i-1) / WIDTH - 1] |= (u64)(bitstring[slen - i] - '0') << (i-1) % WIDTH;
    }
  }
  if (i <= slen) {
#ifdef DEBUG
    printf("binary bigger than wordsize\n");
#endif
    return ERROR;
  }
  memcpy(out, num, SIZE_BYTES);
  return SUCCESS;
}

static int decstring_to_u64(const char *decstring, int wordsize, u64 *out) {
  u64 sum = 0;
  char c;
  int slen = strlen(decstring);
  u64 factor = 1ULL;
  printf("slen=%d\n", slen);
  for (int i = slen - 1; i >= 0; i--) {
    c = decstring[i];
    sum += factor*(c-'0');
    if (sum > get_max_unsigned(wordsize)) {
#ifdef DEBUG
      printf("Decimal bigger than wordsize\n");
#endif
      return ERROR;
    }
    factor *= 10;
  }
  *out = sum;
  return SUCCESS;
}

static int hexstring_to_u64(const char *hexstring, int wordsize, u64 *out) {
  u64 num[SIZE] = {0};
  int i = 0;
  int slen = strlen(hexstring);
  u64 nibble = 0;

  for (i = 1; i <= slen && i <= wordsize; i++) {
    nibble = get_nibble_val(hexstring[slen - i]);
    num[SIZE - (i*4)/WIDTH - 1] |= nibble << (u64)((4ULL * ((u64)i - 1ULL)) % WIDTH);
  }

  // TODO actually think about this logic
  if (0) {
#ifdef DEBUG
    printf("Hex bigger than wordsize\n");
#endif
    return ERROR;
  }

  memcpy(out, num, SIZE_BYTES);
  return SUCCESS;
}

static u64 get_nibble_val(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  else if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  else if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  else
    return 0;
}

static int bubble_up_overflows(number_t *out, number_t *a, number_t *b) {
  out->metadata.SIGNED_OVERFLOW |= a->metadata.SIGNED_OVERFLOW;
  out->metadata.UNSIGNED_OVERFLOW |= a->metadata.UNSIGNED_OVERFLOW;
  if (NULL != b) {
    out->metadata.SIGNED_OVERFLOW |= b->metadata.SIGNED_OVERFLOW;
    out->metadata.UNSIGNED_OVERFLOW |= b->metadata.UNSIGNED_OVERFLOW;
  }
  return SUCCESS;
}

//int number_getSdec(int64_t *out, number_t *number) {
//  int ws = number->wordsize;
//  u64 num = number->num;
//  if ((num & (1ULL << (ws - 1ULL))) < 1ULL) {
//    *out = (int64_t)num;
//    return SUCCESS;
//  }
//  u64 mask = (1ULL << (ws - 1)) - 1ULL;
//  *out = -(1ULL << (ws - 1ULL)) + (num & mask);
//  return SUCCESS;
//}

void delete_number(number_t *number) {
  if (number) {
    free(number);
  }
}

void free_numbers(void) {}

void number_print(number_t *number) {
  printf("--------------\n");
#ifdef DEBUG
  printf("NUMBER %p\n", number);
#endif
  printf("WORDSIZE %d\n", number->wordsize);
  printf("raw_struct: { %d, { %llx, %llx }, { %u, %u } }\n", number->wordsize, number->num[0], number->num[1], number->metadata.SIGNED_OVERFLOW, number->metadata.UNSIGNED_OVERFLOW);
  printf("BITSTRING: ");
  print_u64(number->num, number->wordsize);
  printf("\n");
  //int64_t sdec = 0;
  //number_getSdec(&sdec, number);
  //printf("Integer Value: %lld\n", (long long)sdec);
  //printf("Unsigned Integer Value: %llu\n", (unsigned long long)number->num);
  printf("Hexadecimal Value: ");
  print_hex(number->num, number->wordsize);
  printf("\n");
  printf("--------------\n");
}

static void print_u64(u64 *num, int wordsize) {
  printf("0b");
  for (int i = 1; i <= wordsize; i++) {
    u64 mask = 1ULL << (wordsize - i)%WIDTH;
    printf("%c", '0' + ((num[SIZE - (wordsize - i) / WIDTH - 1]  & mask) > 0));
  }
}

static void print_hex(u64 *num, int wordsize) {
  printf("0x");
  u64 nibble, mask;
  int round_up = ((wordsize-1) / 4 + 1)* 4;
  for (int i = 1; i <= round_up; i += 4) {
    mask = 0xfULL << (round_up - i - 3)%WIDTH;
    nibble = (mask & num[SIZE - (wordsize - i)/WIDTH - 1]) >> (round_up - i-3);
    if (nibble > 15)
      printf("wtf\n");
    else if (nibble > 9)
      printf("%c", 'a' - 10 + (int)nibble);
    else
      printf("%c", '0' + (int)nibble);
  }
}

int copy_number(number_t *out, number_t *number, int wordsize) {
  if (wordsize == 0) {
    printf("don't do this\n");
    exit(69);
  } else
    out->wordsize = wordsize;
  memcpy(out->num, number->num, SIZE_BYTES);
  return SUCCESS;
}

/*********************************************/
/*********** OPERATIONS **********************/
/*********************************************/

int ones_comp(number_t *out, number_t *num, int wordsize) {
  if (out == NULL || num == NULL)
    return ERROR;
  out->wordsize = wordsize;
  for (int i = 0; i < SIZE; i++)
    out->num[i] = ~num->num[i];
  return SUCCESS;
}

int twos_comp(number_t *out, number_t *num, int wordsize) {
  if (out == NULL || num == NULL)
    return ERROR;
  if (num->num[SIZE - (wordsize-1)/WIDTH - 1] != (1ULL << (wordsize - 1)) % WIDTH)
    bubble_up_overflows(out, num, NULL);
  out->wordsize = wordsize;
  number_t ones;
  ones_comp(&ones, num, wordsize);
  add(out, &ones, &_one_, wordsize);
  return SUCCESS;
}

int add(number_t *out, number_t *a, number_t *b, int wordsize) {
  if (!a || !b | !out) {
    perror("Can't add a NULL number(s)");
    return ERROR;
  }
#ifdef DEBUG
  printf("asize: %d; bsize: %d, prog_data->ws: %d\n", a->wordsize, b->wordsize,
         wordsize);
#endif
  out->wordsize = wordsize;
  int carry_out = 0;
  for (int i = SIZE - 1; i >= 0; i--) {
    out->num[i] = a->num[i] + b->num[i] + carry_out;
    carry_out = a->num[i] ? out->num[i] <= b->num[i] + carry_out : out->num[i] < b->num[i] + carry_out;
  }
  //out->num = (a->num + b->num) & MASK;
  bubble_up_overflows(out, a, b);
  u64 aMSB = a->num[SIZE - (wordsize - 1) / WIDTH - 1] & (1ULL << (wordsize - 1) % WIDTH);
  u64 bMSB = b->num[SIZE - (wordsize - 1)/ WIDTH - 1] & (1ULL << (wordsize - 1) % WIDTH);
  u64 oMSB = out->num[SIZE - (wordsize - 1) / WIDTH - 1] & (1ULL << (wordsize - 1) % WIDTH);
  if ((oMSB && !bMSB && !aMSB) || (!oMSB && aMSB && bMSB))
    out->metadata.SIGNED_OVERFLOW = 1;
  return SUCCESS;
}

int sub(number_t *out, number_t *a, number_t *b, int wordsize) {
  printf("line=%d\n", __LINE__);
  number_t neg_a = {0};
  neg_a.wordsize = a->wordsize;
  printf("line=%d\n", __LINE__);
  if (ERROR == twos_comp(&neg_a, a, a->wordsize))
    return ERROR;
  printf("line=%d\n", __LINE__);
  if (ERROR == add(out, b, &neg_a, wordsize))
    return ERROR;
  printf("line=%d\n", __LINE__);
  return SUCCESS;
}

int lshift(number_t *out, number_t *number, number_t *positions, int wordsize) {
 bubble_up_overflows(out, number, positions);
 // check for unsigned overflows
 u64 lshift = positions->num[SIZE - 1];
 number_t max_num = ZERO(wordsize), shift_max_num = ZERO(wordsize);
 get_max_number(&max_num, wordsize);
 rshift(&shift_max_num, &max_num, positions, wordsize);
 if (greater_than(number, &shift_max_num)) {
#ifdef DEBUG
   printf("%s: unsigned overflow w/ num=\n", __FUNCTION__);
   number_print(number);
   printf("shift=\n");
   number_print(positions);
#endif
   out->metadata.UNSIGNED_OVERFLOW = 1;
 }
 out->wordsize = wordsize;
 u64 shift_in = 0;
 for (int i = SIZE-1; i >= 0; i--) {
   out->num[i] = (number->num[i] << lshift) | (shift_in >> (WIDTH - lshift));
   shift_in = (((1ULL << lshift) - 1) << (WIDTH - lshift)) & number->num[i];
 }
 return SUCCESS;
}

int rshift(number_t *out, number_t *number, number_t *positions, int wordsize) {
  bubble_up_overflows(out, number, positions);
  // TODO this doesnt work for more than 64
  // TODO: use get_dec eventually
  u64 rshift = positions->num[SIZE - 1];
  if (rshift >= SIZE * WIDTH) {
#ifdef DEBUG
      printf("%s: shifting_value=0x%llx is greater than max number of bits\n", __FUNCTION__, rshift);
#endif
      memset(out->num, 0, SIZE_BYTES);
  }
  out->wordsize = wordsize;
  // make sure to zero out the most significant bits
  u64 new = 0;
  out->num[0] = (number->num[0] & ((1ULL << wordsize % WIDTH) - 1)) >> rshift;
  u64 shift_in = number->num[0] & ((1ULL << rshift) - 1);
  for (int i = 1; i < SIZE; i++) {
    new = number->num[i] >> rshift;
    new |= shift_in << (WIDTH - rshift);
    shift_in = number->num[i] & ((1ULL << rshift) - 1);
    out->num[i] = new;
  }
  return SUCCESS;
}

/*         compare        */
static int compare(number_t *a, number_t *b) {
  if (!a || !b) {
    printf("%s: null param\n", __FUNCTION__);
    return 0;
  }
  for (int i = 0; i < SIZE; i++) {
    if (a->num[i] > b->num[i])
      return 1;
    if (a->num[i] < b->num[i])
      return -1;
  }
  // they are equal!
  return 0;
}

int greater_than(number_t *a, number_t *b) {
  return 1 == compare(a, b);
}

/*           and             */
int and (number_t * out, number_t *a, number_t *b, int wordsize) {
  if (out == NULL || a == NULL || b == NULL)
    return ERROR;
  bubble_up_overflows(out, a, b);
  out->wordsize = wordsize;
  for (int i = 0; i < SIZE; i++)
    out->num[i] = a->num[i] & b->num[i];
  return SUCCESS;
}

/*             or              */
int or (number_t * out, number_t *a, number_t *b, int wordsize) {
  if (out == NULL || a == NULL || b == NULL)
    return ERROR;
  bubble_up_overflows(out, a, b);
  for (int i = 0; i < SIZE; i++)
    out->num[i] = a->num[i] | b->num[i];
  return SUCCESS;
}

int numbers_are_equal(number_t *a, number_t *b) {
  for (int i = 0; i < SIZE; i++) {
    if (a->num[i] != b->num[i]) {
#ifdef UNIT_TEST
      printf("CASE FAILED\n");
#endif
      return 1;
    }
  }
#ifdef UNIT_TEST
  printf("CASE PASSED\n");
#endif
  return 0;
}

static u64 get_max_unsigned(int wordsize) {
  if (wordsize < 64)
    return (1ULL << wordsize) - 1ULL;
  return UINT64_MAX;
}

static u64 get_max_number(number_t *out, int wordsize) {
  out->wordsize = wordsize;
  int current_ws = wordsize;
  int n_ptr = SIZE - 1;
  while (current_ws > 0) {
    out->num[n_ptr--] = current_ws >= WIDTH ? UINT64_MAX : (1ULL << current_ws) - 1;
    current_ws -= WIDTH;
  }
  return SUCCESS;
}

/***********************************/
/*********       TESTS     *********/
/***********************************/

#ifdef UNIT_TEST

int isEqualToBitstring(number_t *n, char *s) {
  if (!n || !s) {
    printf("%s: either number or bitstring was null\n", __FUNCTION__);
    return 1;
  }
  int len = strlen(s);

  for (int i = 1; i <= len; i++) {
    if ((n->num[SIZE - (n->wordsize - i - 1) / WIDTH - 1] & (1ULL << (i - 1) % WIDTH)) >> (i - 1) % WIDTH != s[len - i] - '0') {
      printf("%s: incorrect %d-th bit\n", __FUNCTION__, i);
      return 1;
    }
  }
  return 0;
}

int test_twos_comp(char *num, char *expected, int wordsize, char *msg) {
  printf("_____ TWO's COMPLEMENT(num) with %d-bit words _____\n", wordsize);
  if (msg != NULL)
    printf("Objective: %s\n", msg);
  number_t n;
  number_t twos;
  new_number(&n, BINARY, num, wordsize);
  printf("num = ");
  print_u64(n.num, wordsize);
  printf("\n");
  printf("expected two's complement = %s\n", expected);
  twos_comp(&twos, &n, wordsize);
  printf("actual two's complement = ");
  print_u64(twos.num, wordsize);
  printf("\n");
  int ret = isEqualToBitstring(&twos, expected);
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed!\n");
  return ret;
}

int test_lshift(char *num, char *pos, char *expected, int wordsize, char *msg) {
 printf("_____ LSHIFT num << pos (%d-bit Numbers) _____\n", wordsize);
 if (msg != NULL)
   printf("Objective: %s\n", msg);
 number_t p, n, res;
 new_number(&n, BINARY, num, wordsize);
 new_number(&p, BINARY, pos, wordsize);
 printf("num = ");
 print_u64(n.num, wordsize);
 printf("\n");
 printf("pos = ");
 print_u64(p.num, wordsize);
 printf("\n");
 printf("expected num << pos = %s\n", expected);
 lshift(&res, &n, &p, wordsize);
 printf("actual num << pos = ");
 print_u64(res.num, wordsize);
 printf("\n");
 int ret = isEqualToBitstring(&res, expected);
 if (!ret)
   printf("Test Passed!\n");
 else
   printf("Test Failed!\n");
 return ret;
}

int test_rshift(char *num, char *pos, char *expected, int wordsize, char *msg) {
  printf("_____ RSHIFT num >> pos (%d-bit Numbers) _____\n", wordsize);
  if (msg != NULL)
    printf("Objective: %s\n", msg);
  number_t n, p, res;
  new_number(&n, BINARY, num, wordsize);
  new_number(&p, BINARY, pos, wordsize);
  printf("num = ");
  print_u64(n.num, wordsize);
  printf("\n");
  printf("pos = ");
  print_u64(p.num, wordsize);
  printf("\n");
  printf("expected num >> pos = %s\n", expected);
  rshift(&res, &n, &p, wordsize);
  printf("actual num >> pos = ");
  print_u64(res.num, wordsize);
  printf("\n");
  int ret = isEqualToBitstring(&res, expected);
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed!\n");
  return ret;
}

int test_add(char *aS, int aWs, char *bS, int bWs, int oWs, char *expected,
             char *msg) {
  printf("_____ ADD a+b (%d-bit + %d-bit) _____\n", aWs, bWs);
  if (msg != NULL)
    printf("Objective: %s\n", msg);
  number_t a, b, sum;
  new_number(&a, BINARY, aS, aWs);
  new_number(&b, BINARY, bS, bWs);
  printf("a = ");
  print_u64(a.num, aWs);
  printf("\n");
  printf("b = ");
  print_u64(b.num, bWs);
  printf("\n");
  printf("expected a+b = %s\n", expected);
  add(&sum, &a, &b, oWs);
  printf("actual a+b = ");
  print_u64(sum.num, oWs);
  printf("\n");
  int ret = isEqualToBitstring(&sum, expected);
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed!\n");
  return ret;
}

//int test_copy_number(char *num, int iws, int ows, char *expected, char *msg) {
//  printf("_____ COPY num (%d-bit -> %d-bit) _____\n", iws, ows);
//  if (msg != NULL)
//    printf("Objective: %s\n", msg);
//  number_t n, new_n;
//  new_number(&n, BINARY, num, iws);
//  printf("num = ");
//  print_u64(n.num, iws);
//  printf("\n");
//  printf("expected new_num = %s\n", expected);
//  copy_number(&new_n, &n, ows);
//  printf("actual new_num = ");
//  print_u64(new_n.num, ows);
//  printf("\n");
//  int ret = isEqualToBitstring(&new_n, expected);
//  if (!ret)
//    printf("Test Passed!\n");
//  else
//    printf("Test Failed :(\n");
//  return ret;
//}

int test_sub(char *aS, int aWs, char *bS, int bWs, int oWs, char *expected,
             char *msg) {
  printf("_____ SUB b-a (%d-bit - %d-bit) _____\n", bWs, aWs);
  if (msg != NULL)
    printf("Objective: %s\n", msg);
  number_t a = {0}, b = {0}, sum = {0};
  new_number(&a, BINARY, aS, aWs);
  new_number(&b, BINARY, bS, bWs);
  number_print(&a);
  number_print(&a);
  printf("b = ");
  print_u64(b.num, bWs);
  printf("\n");
  printf("a = ");
  print_u64(a.num, aWs);
  printf("\n");
  printf("expected b-a = %s\n", expected);
  sub(&sum, &a, &b, oWs);
  printf("actual b-a = ");
  print_u64(sum.num, oWs);
  printf("\n");
  int ret = isEqualToBitstring(&sum, expected);
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed!\n");
  return ret;
}

int test_and(int ws, char *aS, int aWs, char *bS, int bWs, char *expected,
             char *msg) {
  printf("_____ AND a&b (%d-bit - %d-bit) _____\n", aWs, bWs);
  if (msg != NULL)
    printf("Objective: %s\n", msg);
  number_t a, b, anded;
  new_number(&a, BINARY, aS, aWs);
  new_number(&b, BINARY, bS, bWs);
  printf("b = ");
  print_u64(b.num, bWs);
  printf("\n");
  printf("a = ");
  print_u64(a.num, aWs);
  printf("\n");
  printf("expected a&b = %s\n", expected);
  and(&anded, &a, &b, ws);
  number_print(&anded);
  printf("actual a&b = ");
  print_u64(anded.num, ws);
  printf("\n");
  int ret = isEqualToBitstring(&anded, expected);
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed!\n");
  return ret;
}
int test_or(int ws, char *aS, int aWs, char *bS, int bWs, char *expected,
            char *msg) {
  printf("_____ OR a|b (%d-bit | %d-bit) _____\n", aWs, bWs);
  if (msg != NULL)
    printf("Objective: %s\n", msg);
  number_t a, b, ored;
  new_number(&a, BINARY, aS, aWs);
  new_number(&b, BINARY, bS, bWs);
  printf("a = ");
  print_u64(a.num, aWs);
  printf("\n");
  number_print(&a);
  printf("b = ");
  print_u64(b.num, bWs);
  printf("\n");
  number_print(&b);
  printf("expected a|b = %s\n", expected);
  or (&ored, &a, &b, ws);
  printf("actual a|b = ");
  print_u64(ored.num, ws);
  printf("\n");
  number_print(&ored);
  int ret = isEqualToBitstring(&ored, expected);
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed!\n");
  return ret;
}

#endif
