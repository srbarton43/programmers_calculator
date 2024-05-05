// A THOUGHT
// operations could store result in second param? like asm

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "number.h"
#include "utils.h"

#define MASK ((1ULL << wordsize) - 1)

number_t _zero_;
number_t _one_;

static void print_u64(u64 num, int wordsize);
static int bits_to_u64(const char *bitstring, int wordsize, u64 *out);
static int hex_to_u64(const char *hexstring, int wordsize, u64 *out);
static u64 get_nibble_val(char c);

// TODO check return code;
int new_number(number_t *out, type_e type, const char *number, int wordsize) {
  if (number == NULL) {
    perror("null number string");
    return ERROR;
  }
  number_t *new_num = out;
  new_num->wordsize = wordsize;
  new_num->num = 0;

  int slen = strlen(number);
  if (slen == 1 && number[0] == '0') {
    // the number is zero
  } else {
    // check return codes here
    switch (type) {
    case BINARY: {
      bits_to_u64(number, wordsize, &new_num->num);
      break;
    }
    case DECIMAL: {
      new_num->num = atoll(number);
      break;
    }
    case HEXADECIMAL: {
      hex_to_u64(number, wordsize, &new_num->num);
      break;
    }
    default:
      perror("new_number: not a supported number type");
      free(new_num);
      return ERROR;
    }
  }
  return SUCCESS;
}

static int bits_to_u64(const char *bitstring, int wordsize, u64 *out) {
  u64 num = 0;
  int slen = strlen(bitstring);

  int i;
  for (i = 1; i <= wordsize; i++) {
    if (i <= slen)
      num |= (bitstring[slen - i] - '0') << (i - 1);
  }
  if (i <= slen) {
    printf("binary bigger than wordsize\n");
    return ERROR;
  }

  *out = num;
  return SUCCESS;
}

static int hex_to_u64(const char *hexstring, int wordsize, u64 *out) {
  u64 num = 0;
  int i = 0;
  int slen = strlen(hexstring);
  u64 nibble = 0;

  for (i = 1; i <= slen && i <= wordsize; i++) {
    nibble = get_nibble_val(hexstring[slen - i]);
    num |= nibble << (u64)(4ULL * ((u64)i - 1ULL));
  }

  if (i < slen) {
    printf("hex bigger than wordsize\n");
    return ERROR;
  }

  *out = num;
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

void change_wordsize(number_t *num, int wordsize) { return; }

int number_getSdec(int64_t *out, number_t *number) {
  int ws = number->wordsize;
  u64 num = number->num;
  if ((num & (1ULL << (ws - 1ULL))) < 1ULL)
    return (int64_t)num;
  u64 mask = (1ULL << (ws - 1)) - 1ULL;
  *out = -(1ULL << (ws - 1ULL)) + (num & mask);
  return SUCCESS;
}

char *number_getHex_u64(number_t *number) {
  u64 num = number->num;
  int ws = number->wordsize;
  char *hex = calloc(ws / 4 + 3, sizeof(char));
  u64 nibble = 0;
  int np = ws % 4 == 0 ? ws / 4 - 1 : ws / 4;
  int hp = 2;
  for (; np >= 0; np--) {
    nibble = (0xf << (np * 4) & num) >> (np * 4);
    if (nibble > 9)
      hex[hp++] = 'a' + nibble - 10;
    else
      hex[hp++] = '0' + nibble;
  }
  hex[0] = '0';
  hex[1] = 'x';
  return hex;
}

void delete_number(number_t *number) {
  if (number) {
    free(number);
  }
}

void init_numbers(void) {
  new_number(&_zero_, BINARY, "0", 1);
  new_number(&_one_, BINARY, "1", 2);
}

void free_numbers(void) {}

void number_print(number_t *number) {
  printf("--------------\n");
#ifdef DEBUG
  printf("NUMBER %p\n", number);
#endif
  printf("WORDSIZE %d\n", number->wordsize);
  // printf("BITSTRING "); printBits(number); printf("\n");
  printf("BITSTRING: ");
  print_u64(number->num, number->wordsize);
  printf("\n");
  int64_t sdec = 0;
  number_getSdec(&sdec, number);
  printf("Integer Value: %lld\n", sdec);
  printf("Unsigned Integer Value: %llu\n", number->num);
  char *hex_u64 = number_getHex_u64(number);
  printf("Hexadecimal Value: %s\n", hex_u64);
  free(hex_u64);
  printf("--------------\n");
}

static void print_u64(u64 num, int wordsize) {
  for (int i = 1; i <= wordsize; i++) {
    u64 mask = 1ULL << (u64)(wordsize - i);
    printf("%c", '0' + ((num & mask) > 0));
  }
}

int copy_number(number_t *out, number_t *number, int wordsize) {
  if (wordsize == 0) {
    printf("don't do this\n");
    exit(69);
  } else
    out->wordsize = wordsize;
  out->num = number->num & MASK;
  return SUCCESS;
}

/*********************************************/
/*********** OPERATIONS **********************/
/*********************************************/

int ones_comp(number_t *out, number_t *num, int wordsize) {
  if (out == NULL || num == NULL)
    return ERROR;
  out->wordsize = wordsize;
  out->num = ~num->num & MASK;
  return SUCCESS;
}
int twos_comp(number_t *out, number_t *num, int wordsize) {
  if (out == NULL || num == NULL)
    return ERROR;
  out->wordsize = wordsize;
  out->num = ~num->num + 1 & MASK;
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
  out->num = a->num + b->num & MASK;
  global_nums_flag.sign = (out->num & (1ULL << (wordsize - 1))) > 1;
  // TODO:
  global_nums_flag.overflow = 0;
  return SUCCESS;
}

int sub(number_t *out, number_t *a, number_t *b, int wordsize) {
  number_t neg_a;
  if (ERROR == twos_comp(&neg_a, a, a->wordsize))
    return ERROR;
  if (ERROR == add(out, b, &neg_a, wordsize))
    return ERROR;

  return SUCCESS;
}

int lshift(number_t *out, number_t *number, number_t *positions, int wordsize) {
  int pos = positions->num;
  if (pos < 0) {
    printf("pos must be positive\n");
    return ERROR;
  }
  out->wordsize = wordsize;
  out->num = number->num << positions->num & MASK;
  return SUCCESS;
}

int rshift(number_t *out, number_t *number, number_t *positions, int wordsize) {
  int pos = positions->num;
  if (pos < 0) {
    printf("pos must be positive\n");
    return ERROR;
  }
  out->wordsize = wordsize;
  out->num = number->num >> positions->num & MASK;
  return SUCCESS;
}

/*           and             */
int and(number_t *out, number_t *a, number_t *b, int wordsize) {
  if (out == NULL || a == NULL || b == NULL)
    return ERROR;
  out->wordsize = wordsize;
  out->num = a->num & b->num & MASK;
  return SUCCESS;
}

/*             or              */
int or(number_t *out, number_t * a, number_t *b, int wordsize) {
  if (out == NULL || a == NULL || b == NULL)
    return ERROR;
  out->wordsize = wordsize;
  out->num = (a->num | b->num) & MASK;
  return SUCCESS;
}

int numbers_are_equal(number_t *a, number_t *b) {
  if (!a || !b) {
    perror("One of the numbers is NULL");
    return 0;
  }
  return a->num == b->num;
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

  for (int i = 1; i <= len; i++) {
    if ((n->num & (1ULL << (i - 1))) >> (i - 1ULL) != s[len - i] - '0') {
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

int test_add(char *aS, int aWs, char *bS, int bWs, char *expected, char *msg) {
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
  add(&sum, &a, &b, max(aWs, bWs));
  printf("actual a+b = ");
  print_u64(sum.num, max(aWs, bWs));
  printf("\n");
  int ret = isEqualToBitstring(&sum, expected);
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed!\n");
  return ret;
}

int test_copy_number(char *num, int iws, int ows, char *expected, char *msg) {
  printf("_____ COPY num (%d-bit -> %d-bit) _____\n", iws, ows);
  if (msg != NULL)
    printf("Objective: %s\n", msg);
  number_t n, new_n;
  new_number(&n, BINARY, num, iws);
  printf("num = ");
  print_u64(n.num, iws);
  printf("\n");
  printf("expected new_num = %s\n", expected);
  copy_number(&new_n, &n, ows);
  printf("actual new_num = ");
  print_u64(new_n.num, ows);
  printf("\n");
  int ret = isEqualToBitstring(&new_n, expected);
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed :(\n");
  return ret;
}

int test_sub(char *aS, int aWs, char *bS, int bWs, char *expected, char *msg) {
  printf("_____ SUB b-a (%d-bit - %d-bit) _____\n", bWs, aWs);
  if (msg != NULL)
    printf("Objective: %s\n", msg);
  number_t a, b, sum;
  new_number(&a, BINARY, aS, aWs);
  new_number(&b, BINARY, bS, bWs);
  printf("b = ");
  print_u64(b.num, bWs);
  printf("\n");
  printf("a = ");
  print_u64(a.num, aWs);
  printf("\n");
  printf("expected b-a = %s\n", expected);
  sub(&sum, &a, &b, max(aWs, bWs));
  printf("actual b-a = ");
  print_u64(sum.num, max(aWs, bWs));
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
