#include "number.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MASK get_max_unsigned(wordsize)
// size for num rep. arr.
#define WIDTH 64
#define SIZE_BYTES SIZE *WIDTH / 8
#define ZERO(ws) \
  {              \
      ws, {0}, {0}}
#define LOW_32(x) (x & 0xffffffff)
#define HIGH_32(x) (x >> 32)
#define TRUE 1
#define FALSE 0

// Implementation uses the complete definition from number.h
typedef struct number {
  int wordsize;          // wordsize for the bitstring
  u64 num[SIZE];         // stores bitstring (only conisider [wordsize] LSB's
  struct {
    unsigned short UNSIGNED_OVERFLOW  : 1;
    unsigned short SIGNED_OVERFLOW    : 1;
    unsigned short INTERPRET_SIGNED   : 1;
  } metadata; // stores number metadata about overflow, etc
} number_t;

typedef uint32_t u32;

// Export these constants for use in other files
number_t _zero_ = {1, {0}, {0, 0}};
number_t _one_ = {2, {0, 1}, {0, 0}};

static const number_t MAX_DECIMAL = {
    128, {0x4ee2d6d415b, 0x85acef80ffffffff}, {0, 0}};

static int u64_multiply(u64 *const high_dig, u64 *const low_dig, u64 a, u64 b);
static u64 u64_half_adder(u64 a, u64 b, u64 *const carry);
static int get_num_digits(int size, u64 *number);

int modulo(number_t *out, number_t *denominator, number_t *numerator, int wordsize);
int divide(number_t *out, number_t *denominator, number_t *numerator, int wordsize);
static int div_and_mod(number_t *quotient, number_t *modulus, number_t *denominator,
                       number_t *numerator, int wordsize);

static void print_bitstring(u64 *num, int wordsize);
static void print_hex(u64 *num, int wordsize);
static int bitstring_to_number(const char *bitstring, int wordsize, number_t *out);
static int hexstring_to_number(const char *hexstring, int wordsize, number_t *out);
static int decstring_to_number(const char *decstring, int wordsize, number_t *out);
static int bubble_up_metadata(number_t *out, number_t *a, number_t *b);
static u64 get_nibble_val(char c);
static int compare(const number_t *a, const number_t *b);
static int get_max_number(number_t *out, int wordsize);
static int zero_number(number_t *out);
static void u32_lshift(u32 *arr, unsigned char shift);
static int u32_lesser_than(u32 *left, u32 *right);
static void u32_subtract(u32 *left, u32 *right);
static void print_decimal(number_t *number, int is_signed);


uint32_t n_sizeof() {
  return sizeof(number_t);
}

int number_alloc(number_t **out) {
  *out = calloc(1, sizeof(number_t));
  return out != NULL;
}

int number_destroy(number_t *num) {
  if (num) {
    free(num);
    return SUCCESS;
  }
  return ERROR;
}

int number_zero(number_t * num) {
  if (num != NULL) {
    *num = _zero_;
  }
  return ERROR;
}

unsigned short n_UNSIGNED_OVERFLOW(number_t *num) {
  return num->metadata.UNSIGNED_OVERFLOW;
}

unsigned short n_SIGNED_OVERFLOW(number_t *num) {
  return num->metadata.SIGNED_OVERFLOW;
}

u64 n_getLeastSigChunk(number_t *out) {
  return out->num[SIZE-1];
}

int new_number(number_t *out, type_e type, const char *number, int wordsize) {
  if (number == NULL) {
    perror("null number string");
    return ERROR;
  }
#ifdef DEBUG
  printf("%s: type=%d, number=%s, wordsize=%d\n", __FUNCTION__, type, number,
         wordsize);
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
        if (ERROR == bitstring_to_number(number, wordsize, new_num))
          new_num->metadata.UNSIGNED_OVERFLOW = 1;
        break;
      }
      case DECIMAL: {
        if (ERROR == decstring_to_number(number, wordsize, new_num))
          new_num->metadata.UNSIGNED_OVERFLOW = 1;
        break;
      }
      case HEXADECIMAL: {
        if (ERROR == hexstring_to_number(number, wordsize, new_num))
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

static int bitstring_to_number(const char *bitstring, int wordsize, number_t *out) {
  uint16_t n_bits = strlen(bitstring);
  if (n_bits > wordsize) {
    return ERROR;
  }
  for (int i = 1; i <= wordsize; i++) {
    if (i <= n_bits) {
      out->num[SIZE - (i - 1) / WIDTH - 1] |= (u64)(bitstring[n_bits - i] - '0')
                                         << (i - 1) % WIDTH;
    }
  }
  return SUCCESS;
}

static int decstring_to_number(const char *decstring, int wordsize, number_t *out) {
  uint16_t n_dig = strlen(decstring);
  number_t ten = { 128, {0, 0xa}, {0, 0}};
  int ret = 0;

  // construct array of digits
  unsigned short *dig_arr = malloc(strlen(decstring));
  for (int i = 0; i < n_dig; i++) {
    const char ch = decstring[i];
    dig_arr[i] = ch - '0';
  }

  number_t scratch = {0};
  for (int i = 0; i < n_dig; i++) {
    ret |= multiply(&scratch, &ten, out, wordsize);
    number_t digit_num = {128, {0, dig_arr[i] }, {0, 0}};
    ret |= add(out, &scratch, &digit_num, wordsize);
  }
  
  free(dig_arr);
  return ret;
}

static int hexstring_to_number(const char *hexstring, int wordsize, number_t *out) {
  uint16_t h_len = strlen(hexstring);
  
  // check for too long hexstring
  // simple version
  if ( h_len > (wordsize + 3) / 4 ) {
    return ERROR;
  }
  // complex version
  u64 msn_bit = 64 - __builtin_clzll(get_nibble_val(hexstring[0]));
  if (msn_bit + (h_len - 1) * 4 > wordsize) {
    return ERROR;
  }

  for (int i = 1; i <= h_len; i++) {
    u64 nibble = get_nibble_val(hexstring[h_len - i]);
    out->num[SIZE - ((i - 1) * 4) / WIDTH - 1] |=
        nibble << (u64)((4ULL * ((u64)i - 1ULL)) % WIDTH);
  }
  
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

static int bubble_up_metadata(number_t *out, number_t *a, number_t *b) {
  out->metadata.SIGNED_OVERFLOW |= a->metadata.SIGNED_OVERFLOW;
  out->metadata.UNSIGNED_OVERFLOW |= a->metadata.UNSIGNED_OVERFLOW;
  out->metadata.INTERPRET_SIGNED |= a->metadata.INTERPRET_SIGNED;
  if (NULL != b) {
    out->metadata.SIGNED_OVERFLOW |= b->metadata.SIGNED_OVERFLOW;
    out->metadata.UNSIGNED_OVERFLOW |= b->metadata.UNSIGNED_OVERFLOW;
    out->metadata.INTERPRET_SIGNED |= b->metadata.INTERPRET_SIGNED;
  }
  return SUCCESS;
}

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
  printf("raw_struct: { %d, { %llx, %llx }, { %u, %u, %u } }\n", number->wordsize,
         number->num[0], number->num[1], number->metadata.SIGNED_OVERFLOW,
         number->metadata.UNSIGNED_OVERFLOW, number->metadata.INTERPRET_SIGNED);
#endif
  printf("WORDSIZE %d\n", number->wordsize);
  printf("BITSTRING: ");
  print_bitstring(number->num, number->wordsize);
  printf("\n");
  printf("Integer Value: ");
  print_signed_decimal(number);
  printf("\n");
  printf("Unsigned Integer Value: ");
  print_unsigned_decimal(number);
  printf("\n");
  // int64_t sdec = 0;
  // number_getSdec(&sdec, number);
  // printf("Integer Value: %lld\n", (long long)sdec);
  // printf("Unsigned Integer Value: %llu\n", (unsigned long long)number->num);
  printf("Hexadecimal Value: ");
  print_hex(number->num, number->wordsize);
  printf("\n");
  printf("--------------\n");
}

#ifdef DEBUG
int number_debug(number_t *num) {
  if (!num) {
    printf("(null)\n");
    return 1;
  }
  printf("raw_struct: { %d, { %llx, %llx }, { %u, %u, %u } }\n", num->wordsize,
         num->num[0], num->num[1], num->metadata.SIGNED_OVERFLOW,
         num->metadata.UNSIGNED_OVERFLOW, num->metadata.INTERPRET_SIGNED);
  return 0;
}
#endif

static void print_bitstring(u64 *num, int wordsize) {
  printf("0b");
  for (int i = 1; i <= wordsize; i++) {
    u64 mask = 1ULL << (wordsize - i) % WIDTH;
    printf("%c", '0' + ((num[SIZE - (wordsize - i) / WIDTH - 1] & mask) > 0));
  }
}

static void print_hex(u64 *num, int wordsize) {
  printf("0x");
  u64 nibble, mask;
  int round_up = ((wordsize - 1) / 4 + 1) * 4;
  for (int i = 1; i <= round_up; i += 4) {
    mask = 0xfULL << (round_up - i - 3) % WIDTH;
    nibble =
        (mask & num[SIZE - (wordsize - i) / WIDTH - 1]) >> (round_up - i - 3);
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
  memcpy(&out->metadata, &number->metadata, sizeof(((number_t *)0)->metadata)); // crazy compiler magic
  return SUCCESS;
}

/*********************************************/
/*********** OPERATIONS **********************/
/*********************************************/

int ones_comp(number_t *out, number_t *num, int wordsize) {
  if (out == NULL || num == NULL)
    return ERROR;
  number_t temp = *num;
  zero_number(out);
  out->wordsize = wordsize;
  for (int i = 0; i < SIZE; i++)
    out->num[i] = ~temp.num[i];
  return SUCCESS;
}

int twos_comp(number_t *out, number_t *num, int wordsize) {
  if (out == NULL || num == NULL)
    return ERROR;
  number_t stack_num = *num;
  bubble_up_metadata(out, &stack_num, NULL);
  out->wordsize = wordsize;
  number_t ones = {0};
  ones_comp(&ones, &stack_num, wordsize);
  number_t mask = {0};
  get_max_number(&mask, wordsize);
  and(&ones, &ones, &mask, wordsize);
  add(out, &ones, &_one_, wordsize);
  out->metadata.INTERPRET_SIGNED ^= 1;
  // weird edge case where mag(max_negative_number) > max_pos
  if (equal_to(out, &stack_num)) {
    out->metadata.SIGNED_OVERFLOW ^= stack_num.metadata.SIGNED_OVERFLOW;
  }
  return SUCCESS;
}

int add(number_t *out, number_t *a, number_t *b, int wordsize) {
  if (!a || !b || !out) {
    perror("Can't add a NULL number(s)");
    return ERROR;
  }
  out->wordsize = wordsize;
  int carry_out = 0;
  for (int i = SIZE - 1; i >= 0; i--) {
    out->num[i] = a->num[i] + b->num[i] + carry_out;
    carry_out = a->num[i] ? out->num[i] <= b->num[i] + carry_out
                          : out->num[i] < b->num[i] + carry_out;
  }
  // out->num = (a->num + b->num) & MASK;
  bubble_up_metadata(out, a, b);
  u64 aMSB = a->num[SIZE - (wordsize - 1) / WIDTH - 1] &
             (1ULL << (wordsize - 1) % WIDTH);
  u64 bMSB = b->num[SIZE - (wordsize - 1) / WIDTH - 1] &
             (1ULL << (wordsize - 1) % WIDTH);
  u64 oMSB = out->num[SIZE - (wordsize - 1) / WIDTH - 1] &
             (1ULL << (wordsize - 1) % WIDTH);
  
  if ((oMSB && !bMSB && !aMSB) || (!oMSB && aMSB && bMSB))
    out->metadata.SIGNED_OVERFLOW = 1;
  if (! (a->metadata.INTERPRET_SIGNED ||  b->metadata.INTERPRET_SIGNED)) {
    number_t max_unsigned = {0};
    get_max_number(&max_unsigned, wordsize);
    if (greater_than(out, &max_unsigned))
      out->metadata.UNSIGNED_OVERFLOW = 1;
  }
  return SUCCESS;
}

int sub(number_t *out, number_t *a, number_t *b, int wordsize) {
  number_t neg_a = {0};
  neg_a.wordsize = a->wordsize;
  if (ERROR == twos_comp(&neg_a, a, a->wordsize))
    return ERROR;
  if (ERROR == add(out, b, &neg_a, wordsize))
    return ERROR;
  return SUCCESS;
}

static int get_num_digits(int size, u64 *number) {
  int digits = size;
  for (int i = 0; i < size; i++) {
    if (number[i] == 0)
      digits--;
    else
      break;
  }
  return digits;
}

static u64 u64_half_adder(u64 a, u64 b, u64 *const carry) {
  u64 result = a + b;
  *carry += (result < a ? 1 : 0);
  return result;
}

static int u64_multiply(u64 *const high_dig, u64 *const low_dig, u64 a, u64 b) {
  if (!a || !b) {
    *high_dig = 0;
    *low_dig = 0;
    return SUCCESS;
  }

  u64 a0 = LOW_32(a), a1 = HIGH_32(a);
  u64 b0 = LOW_32(b), b1 = HIGH_32(b);

  u64 a0b0 = a0 * b0;
  u64 a1b0 = a1 * b0;
  u64 a0b1 = a0 * b1;
  u64 a1b1 = a1 * b1;

  u64 c0 = 0, c1 = 0;

  u64 middle_part = u64_half_adder(a0b1, a1b0, &c1);

  *low_dig = u64_half_adder(a0b0, LOW_32(middle_part) << 32, &c0);
  *high_dig = a1b1 + HIGH_32(middle_part) + (c1 << 32) + c0;

  return SUCCESS;
}

int multiply(number_t *out, number_t *a, number_t *b, int wordsize) {
  if (!out || !a || !b)
    return ERROR;

  number_t *big, *small;
  u64 carry_in_mult = 0, carry_out_mult, out_digit;
  int aDigits = get_num_digits(SIZE, a->num);
  int bDigits = get_num_digits(SIZE, b->num);

  if (aDigits > bDigits) {
    big = a;
    small = b;
  } else {
    big = b;
    small = a;
  }

  // calculate the intermediate multiplications into a 2d array
  u64 intermediate_numbers[SIZE][SIZE * 2] = {0};
  for (int i = SIZE - 1; i >= SIZE - get_num_digits(SIZE, small->num); i--) {
    for (int j = SIZE - 1; j >= SIZE - get_num_digits(SIZE, big->num); j--) {
      carry_out_mult = 0;
      u64_multiply(&carry_out_mult, &out_digit, small->num[i], big->num[j]);
      intermediate_numbers[SIZE - i - 1][i + j + 1] =
          u64_half_adder(out_digit, carry_in_mult, &carry_out_mult);
      carry_in_mult = carry_out_mult;
    }
    // carry out
    intermediate_numbers[SIZE - i - 1]
                        [i + (SIZE - get_num_digits(SIZE, big->num))] =
                            carry_out_mult;
  }

#ifdef DEBUG
  printf("intermediate numbers: \n{\n");
  for (int i = 0; i < SIZE; i++) {
    printf("{ ");
    for (int j = 0; j < 2 * SIZE; j++) {
      printf("%llx, ", intermediate_numbers[i][j]);
    }
    printf("}\n");
  }
  printf("}\n");
#endif

  // sum up the intermediate numbers
  u64 result[2 * SIZE] = {0};
  u64 carry_in_add = 0, carry_out_add = 0, vert_sum = 0;
  for (int j = 2 * SIZE - 1; j >= 0; j--) {
    vert_sum = 0, carry_out_add = 0;
    for (int i = 0; i < SIZE; i++) {
      vert_sum += u64_half_adder(intermediate_numbers[i][j], carry_in_add,
                                 &carry_out_add);
      carry_in_add = 0;
    }
    carry_in_add = carry_out_add;
    result[j] = vert_sum;
  }

#ifdef DEBUG
  printf("result: \n{ ");
  for (int i = 0; i < 2 * SIZE; i++) {
    printf("%llx, ", result[i]);
  }
  printf(" }\n");
#endif

  // read into the output digits array
  for (int i = 2 * SIZE - 1; i >= SIZE; i--) {
    out->num[i - SIZE] = result[i];
  }

  // detect overflow
  out->wordsize = wordsize;
  if (get_num_digits(2 * SIZE, result) > SIZE) {
    out->metadata.UNSIGNED_OVERFLOW = 1;
  } else {
    number_t max_number = {0};
    get_max_number(&max_number, wordsize);
    if (greater_than(out, &max_number))
      out->metadata.UNSIGNED_OVERFLOW = 1;
    else if ((1ULL << (wordsize - 1) % WIDTH &
              out->num[SIZE - wordsize / WIDTH - 1]) > 0) {
      out->metadata.SIGNED_OVERFLOW = 1;
    }
  }

  return SUCCESS;
}

static void u32_lshift(u32 *arr, unsigned char shift) {
#ifdef DEBUG
  printf("Entering %s\n", __FUNCTION__);
  printf("shift=%d\n", shift);
#endif
  u32 shift_in = 0;
  u32 copy_arr[2 * SIZE];
  memcpy(copy_arr, arr, sizeof(copy_arr));
  for (int i = 0; i < shift / (WIDTH / 2); i++) {
    arr[2 * SIZE - 1 - i] = 0;
  }
  for (int i = 2 * SIZE - 1; i >= (int)(shift / (WIDTH / 2)); i--) {
#ifdef DEBUG
    printf("i=%d\tarr_idx=%d\n", i, i - shift / (WIDTH / 2));
    printf("chunk=%x\tshift_in=%x\n", copy_arr[i] << shift % (WIDTH / 2), shift_in);
#endif
    arr[i - shift / (WIDTH / 2)] =
        (copy_arr[i] << shift % (WIDTH / 2)) |
        (shift_in >> ((WIDTH / 2 - shift) % (WIDTH / 2)));
    shift_in = (((1U << shift) - 1) << (WIDTH / 2 - shift)) & copy_arr[i];
  }
#ifdef DEBUG
  printf("Exiting %s\n", __FUNCTION__);
#endif
}

static int div_and_mod(number_t *quotient, number_t *remainder,
                       number_t *denominator, number_t *numerator, int wordsize) {
  // Print inputs for debugging
#ifdef DEBUG
  printf("div_and_mod: denominator={%llx,%llx}, numerator={%llx,%llx}\n",
         denominator->num[0], denominator->num[1], numerator->num[0], numerator->num[1]);
  printf("Entering div_and_mod\n");
#endif
  if (!quotient || !remainder || !numerator || !denominator)
    return ERROR;
  if (equal_to(&_zero_, denominator)) {
#ifdef DEBUG
    printf("error: dividing by zero!\n");
#endif
    return ERROR;
  } else if (greater_than(denominator, numerator)) {
    *quotient = _zero_;
    quotient->wordsize = wordsize;
    *remainder = *numerator;
    remainder->wordsize = wordsize;
  } else if (equal_to(denominator, numerator)) {
    *quotient = _one_;
    quotient->wordsize = wordsize;
    *remainder = _zero_;
    remainder->wordsize = wordsize;
  } else {
    // division algorithm
    unsigned char lambda = 0;  // multiply num and den by factor
    *quotient = _zero_;
    quotient->wordsize = wordsize;

    // quotient qrray with half-chunks
    u32 quotient_u32_arr[2 * SIZE] = {0};

    // convert to u32 arr
    u32 denominator_u32_arr[2 * SIZE] = {0};
    for (int i = 0; i < SIZE; i++) {
      denominator_u32_arr[2 * i] = denominator->num[i] >> WIDTH / 2;
      denominator_u32_arr[2 * i + 1] = denominator->num[i];
    }
    u32 numerator_u32_arr[2 * SIZE] = {0};
    for (int i = 0; i < SIZE; i++) {
      numerator_u32_arr[2 * i] = numerator->num[i] >> WIDTH / 2;
      numerator_u32_arr[2 * i + 1] = numerator->num[i];
    }
    int t = 2 * SIZE - 1;
    for (int i = 0; i < 2 * SIZE; i++) {
      if (denominator_u32_arr[i] != 0)
        break;
      t--;
    }
#ifdef DEBUG
    printf("printing u32 arrays\n");
    printf("numerator:\n{ ");
    for (int i = 0; i < 2 * SIZE; i++) {
      printf("%x, ", numerator_u32_arr[i]);
    }
    printf("}\n");
    printf("denominator:\n{ ");
    for (int i = 0; i < 2 * SIZE; i++) {
      printf("%x, ", denominator_u32_arr[i]);
    }
    printf("}\n");
    // get MSB of denominator to be larger than half digit size
    printf("t=%d\n", t);
#endif
    // Special case for division by 1
    if (t == 0 && denominator_u32_arr[2 * SIZE - 1] == 1) {
      *quotient = *numerator;
      quotient->wordsize = wordsize;
      *remainder = _zero_;
      remainder->wordsize = wordsize;
      return SUCCESS;
    }
    if (denominator_u32_arr[2 * SIZE - t - 1] < UINT32_MAX / 2) {
      while ((denominator_u32_arr[2 * SIZE - t - 1] << lambda) < UINT32_MAX / 2)
        lambda++;
#ifdef DEBUG
      printf("lambda = %d\n", lambda);
      printf("new msb = %x\n", denominator_u32_arr[2 * SIZE - t - 1] << lambda);
#endif
      // shift denominator and numerator by lambda
      u32_lshift(denominator_u32_arr, lambda);
      u32_lshift(numerator_u32_arr, lambda);
    }
    int n = 2 * SIZE - 1;
    for (int i = 0; i < 2 * SIZE; i++) {
      if (numerator_u32_arr[i] != 0)
        break;
      n--;
    }

#ifdef DEBUG
    printf("new_n=%d\n", n);

    printf("printing u32 arrays after shifting\n");
    printf("numerator:\n{ ");
    for (int i = 0; i < 2 * SIZE; i++) {
      printf("%x, ", numerator_u32_arr[i]);
    }
    printf("}\n");
    printf("denominator:\n{ ");
    for (int i = 0; i < 2 * SIZE; i++) {
      printf("%x, ", denominator_u32_arr[i]);
    }
    printf("}\n");
#endif

    // if denominator has only one "digit" do simple algo
    if (t == 0) {
#ifdef DEBUG
      printf("denominator has only one (32-bit) \"digit\"\n");
#endif
      u64 denominator_digit = denominator_u32_arr[2 * SIZE - 1];
      u64 u64_remainder = 0;
      u64 current = 0;

      for (int i = 2 * SIZE - n - 1; i < 2 * SIZE; i++) {
        current = (u64_remainder << WIDTH / 2) + numerator_u32_arr[i];
        quotient_u32_arr[i] = (u32)(current / denominator_digit);
        u64_remainder = current % denominator_digit;
#ifdef DEBUG
        printf("i=%d\tcurrent=%llx\tquotient[i]=%x\tremainder=%llx\n", i,
               current, quotient_u32_arr[i], u64_remainder);
#endif
      }
      // put back into u64 arr
      for (int i = 0; i < SIZE; i++) {
        quotient->num[i] = (((u64)quotient_u32_arr[2 * i]) << (WIDTH / 2)) +
                           quotient_u32_arr[2 * i + 1];
      }
      remainder->num[SIZE - 1] = u64_remainder >> lambda;

      quotient->wordsize = wordsize;
      remainder->wordsize = wordsize;
    } else {
#ifdef DEBUG
      printf("denominator has multiple (32-bit) \"digits\"\n");
#endif
      // step 2, align and then subtract denominator from numerator until numerator >=
      // aligned
      u32 aligned_u32_arr[2 * SIZE];
      memcpy(aligned_u32_arr, denominator_u32_arr, sizeof(aligned_u32_arr));
#ifdef DEBUG
      printf("aligned denominator: { ");
      for (int i = 0; i < 2 * SIZE; i++) {
        printf("%x, ", aligned_u32_arr[i]);
      }
      printf(" }\n");
#endif
      for (int i = 2 * SIZE - 1; i >= 2 * SIZE - (n - t); i--) {
        u32_lshift(aligned_u32_arr, WIDTH / 2);
      }
#ifdef DEBUG
      printf("shifted by 32 denominator: { ");
      for (int i = 0; i < 2 * SIZE; i++) {
        printf("%x, ", aligned_u32_arr[i]);
      }
      printf(" }\n");
#endif

#ifdef DEBUG
      printf("numerator: { ");
      for (int i = 0; i < 2 * SIZE; i++) {
        printf("%x, ", numerator_u32_arr[i]);
      }
      printf(" }\n");
#endif

      // do a check here that it is incrementing the correct digit
      while (!u32_lesser_than(numerator_u32_arr, aligned_u32_arr)) {
        quotient_u32_arr[2 * SIZE - (n - t)] += 1;
        u32_subtract(numerator_u32_arr, aligned_u32_arr);
      }

#ifdef DEBUG
      printf("aligned denominator: { ");
      for (int i = 0; i < 2 * SIZE; i++) {
        printf("%x, ", aligned_u32_arr[i]);
      }
      printf("}\n");
      printf("new numerator: { ");
      for (int i = 0; i < 2 * SIZE; i++) {
        printf("%x, ", numerator_u32_arr[i]);
      }
      printf("}\n");
#endif

      // step 3 of Knuth's algorithm D
      u32 x_3digit[2 * SIZE] = {0};
      u32 y_2digit[2 * SIZE] = {0};
      u64 q_u128 = 0;
      u32 q_digit = 0;

#ifdef DEBUG
      printf("Starting step 3: n=%d, t=%d\n", n, t);
#endif

      // step 3: for i = n downto t + 1
      for (int i = n; i >= t + 1; i--) {
#ifdef DEBUG
        printf("Processing digit i=%d\n", i);
#endif
        q_digit = 0;

        // step 3.1: estimate quotient digit
        if (numerator_u32_arr[2 * SIZE - i - 1] == denominator_u32_arr[2 * SIZE - t - 1]) {
          q_digit = UINT32_MAX - 1;
#ifdef DEBUG
          printf("Equal leading digits, q_digit set to: %u\n", q_digit);
#endif
        } else {
          q_u128 = ((u64)numerator_u32_arr[2 * SIZE - i - 1] << 32) |
                   numerator_u32_arr[2 * SIZE - i];
          q_digit = q_u128 / denominator_u32_arr[2 * SIZE - t - 1];
#ifdef DEBUG
          printf("Estimated q_digit: %u (from %llu / %u)\n",
                 q_digit, q_u128, denominator_u32_arr[2 * SIZE - t - 1]);
#endif
        }

        // calculate 3-digit dividend portion and 2-digit divisor portion for comparison
        memset(x_3digit, 0, sizeof(x_3digit));
        x_3digit[2 * SIZE - 1] = numerator_u32_arr[2 * SIZE - i - 1];
        x_3digit[2 * SIZE - 2] = numerator_u32_arr[2 * SIZE - i];
        x_3digit[2 * SIZE - 3] = numerator_u32_arr[2 * SIZE - i + 1];

        memset(y_2digit, 0, sizeof(y_2digit));
        y_2digit[2 * SIZE - 1] = denominator_u32_arr[2 * SIZE - t - 1];
        y_2digit[2 * SIZE - 2] = denominator_u32_arr[2 * SIZE - t];

#ifdef DEBUG
        printf("3-digit x: [%u, %u, %u]\n", x_3digit[2 * SIZE - 1], x_3digit[2 * SIZE - 2], x_3digit[2 * SIZE - 3]);
        printf("2-digit y: [%u, %u]\n", y_2digit[2 * SIZE - 1], y_2digit[2 * SIZE - 2]);
#endif

        // step 3.2: decrease q_digit until q_digit * y_2digit <= x_3digit
        u32 product[2 * SIZE] = {0};
        u32 q_too_large = 1;
        int refinement_count = 0;

        while (q_too_large && q_digit > 0) {
          // calculate q_digit * y_2digit
          memset(product, 0, sizeof(product));
          u64 carry = 0;

          // Multiply each digit of y_2digit by q_digit, starting from lowest digit
          for (int j = 2 * SIZE - 1; j >= 0; j--) {
            if (j <= 2 * SIZE - 3) break;  // Only multiply the 2 digits

            u64 mult = (u64)q_digit * y_2digit[j] + carry;
            product[j] = mult & 0xFFFFFFFF;
            carry = mult >> 32;

            // Handle carry to the next digit
            if (j > 0 && carry > 0) {
              product[j - 1] = carry;
              carry = 0;
            }
          }

          // Compare product with x_3digit (most significant first)
          int compare_result = 0;
          for (int j = 0; j < 2 * SIZE; j++) {
            if (product[j] < x_3digit[j]) {
              compare_result = -1;  // product < x_3digit
              break;
            } else if (product[j] > x_3digit[j]) {
              compare_result = 1;  // product > x_3digit
              break;
            }
          }

          if (compare_result > 0) {
            q_digit--;
            refinement_count++;
          } else {
            q_too_large = 0;
          }
        }

#ifdef DEBUG
        printf("Refined q_digit: %u (after %d refinements)\n", q_digit, refinement_count);
#endif

        // Store quotient digit - adjust indexing here
        int quotient_index = i - t - 1;  // Compute correct index for quotient
        quotient_u32_arr[2 * SIZE - 1 - quotient_index] = q_digit;
#ifdef DEBUG
        printf("Storing q_digit %u at quotient index %d\n", q_digit, 2 * SIZE - 1 - quotient_index);
#endif

        // Calculate shifted divisor (y_shifted = divisor << ((i-t-1) * 32))
        u32 y_shifted[2 * SIZE] = {0};
        memcpy(y_shifted, denominator_u32_arr, sizeof(y_shifted));

        // Shift based on the position (WIDTH/2 bits per position)
        int shift_positions = i - t - 1;
#ifdef DEBUG
        printf("Shifting divisor by %d positions\n", shift_positions);
#endif

        if (shift_positions > 0) {
          // We need to shift right since our arrays are in reverse order (MSB at [0])
          // First, make a copy of the original array
          u32 temp[2 * SIZE] = {0};
          memcpy(temp, y_shifted, sizeof(temp));

          // Zero out the destination array
          memset(y_shifted, 0, sizeof(y_shifted));

          // Copy digits with the shift applied (shift to the right)
          for (int j = 0; j < 2 * SIZE - shift_positions; j++) {
            y_shifted[j + shift_positions] = temp[j];
          }
        }

#ifdef DEBUG
        printf("Shifted divisor: ");
        for (int j = 0; j < 2 * SIZE; j++) {
          printf("%x ", y_shifted[j]);
        }
        printf("\n");
#endif

        // Step 3.3 and 3.4: subtract q_digit * y_shifted from dividend
        u32 q_times_y[2 * SIZE] = {0};

        // Compute q_digit * y_shifted with proper carry handling
        u64 carry = 0;
        for (int j = 2 * SIZE - 1; j >= 0; j--) {
          u64 mult = (u64)q_digit * y_shifted[j] + carry;
          q_times_y[j] = mult & 0xFFFFFFFF;
          carry = mult >> 32;
        }

#ifdef DEBUG
        printf("q_digit * y_shifted: ");
        for (int j = 0; j < 2 * SIZE; j++) {
          printf("%x ", q_times_y[j]);
        }
        printf("\n");

        printf("Current numerator: ");
        for (int j = 0; j < 2 * SIZE; j++) {
          printf("%x ", numerator_u32_arr[j]);
        }
        printf("\n");
#endif

        // Compare numerator with q_times_y (most significant first)
        int compare_result = 0;
        for (int j = 0; j < 2 * SIZE; j++) {
          if (numerator_u32_arr[j] < q_times_y[j]) {
            compare_result = -1;  // numerator < q_times_y
            break;
          } else if (numerator_u32_arr[j] > q_times_y[j]) {
            compare_result = 1;  // numerator > q_times_y
            break;
          }
        }

        if (compare_result >= 0) {
          // Step 3.3: dividend >= q_digit * y_shifted, just subtract
#ifdef DEBUG
          printf("Case 3.3: numerator >= q_times_y, subtracting\n");
#endif
          u32_subtract(numerator_u32_arr, q_times_y);
        } else {
          // Step 3.4: dividend < q_digit * y_shifted, need to add back
#ifdef DEBUG
          printf("Case 3.4: numerator < q_times_y, adding back\n");
#endif

          // Create temp = q_times_y - y_shifted
          u32 temp[2 * SIZE];
          memcpy(temp, q_times_y, sizeof(temp));
          u32_subtract(temp, y_shifted);  // temp = q_times_y - y_shifted

          // Numerator = numerator - temp
          u32_subtract(numerator_u32_arr, temp);

          // Decrement quotient digit
          quotient_u32_arr[2 * SIZE - 1 - quotient_index]--;
#ifdef DEBUG
          printf("Decremented quotient digit to %u\n", quotient_u32_arr[2 * SIZE - 1 - quotient_index]);
#endif
        }

#ifdef DEBUG
        printf("Numerator after step: ");
        for (int j = 0; j < 2 * SIZE; j++) {
          printf("%x ", numerator_u32_arr[j]);
        }
        printf("\n\n");
#endif
      }

#ifdef DEBUG
      printf("Final quotient array: ");
      for (int j = 0; j < 2 * SIZE; j++) {
        printf("%x ", quotient_u32_arr[j]);
      }
      printf("\n");

      // Rewind shifts by lambda to get actual remainder
      printf("Rewinding lambda shifts: %d bits\n", lambda);
#endif

      // Handle right shift for lambda bits
      if (lambda > 0) {
        // For our array representation, we need to right shift since MSB is at index 0
        int full_words = lambda / 32;
        int remaining_bits = lambda % 32;

#ifdef DEBUG
        printf("Right shift: %d words + %d bits\n", full_words, remaining_bits);
#endif

        if (full_words > 0) {
          // Shift full words first
          for (int j = 2 * SIZE - 1; j >= full_words; j--) {
            numerator_u32_arr[j] = numerator_u32_arr[j - full_words];
          }
          // Zero out the words that were shifted out
          for (int j = 0; j < full_words; j++) {
            numerator_u32_arr[j] = 0;
          }
        }

        // Then do bit-by-bit shifting for remaining bits
        if (remaining_bits > 0) {
          for (int i = 0; i < remaining_bits; i++) {
            u32 carry = 0;
            // Process from least significant digit to most significant
            for (int j = 2 * SIZE - 1; j >= 0; j--) {
              u32 next_carry = (numerator_u32_arr[j] & 1) << 31;
              numerator_u32_arr[j] = (numerator_u32_arr[j] >> 1) | carry;
              carry = next_carry;
            }
          }
        }

#ifdef DEBUG
        printf("Remainder after shift: ");
        for (int j = 0; j < 2 * SIZE; j++) {
          printf("%x ", numerator_u32_arr[j]);
        }
        printf("\n");
#endif
      }

      // Convert the quotient and remainder back to u64 format
#ifdef DEBUG
      printf("Converting u32 arrays back to u64\n");
#endif
      for (int i = 0; i < SIZE; i++) {
        quotient->num[i] = ((u64)quotient_u32_arr[2 * i] << 32) | quotient_u32_arr[2 * i + 1];
        remainder->num[i] = ((u64)numerator_u32_arr[2 * i] << 32) | numerator_u32_arr[2 * i + 1];
#ifdef DEBUG
        printf("quotient[%d] = %llx, remainder[%d] = %llx\n", i, quotient->num[i], i, remainder->num[i]);
#endif
      }
    }  // end branch with for multiple 32-bit digits
  }

#ifdef DEBUG
  printf("Exiting div_and_mod\n");
#endif
  return SUCCESS;
}

static void u32_subtract(u32 *left, u32 *right) {
  u32 out[2 * SIZE] = {0};
  u32 temp[2 * SIZE] = {0};

  // flip bits
  for (int i = 0; i < 2 * SIZE; i++) {
    temp[i] = ~right[i];
  }

  // add one
  u32 carry_out = 0;
  for (int i = 2 * SIZE - 1; i >= 0; i--) {
    temp[i] += 1 + carry_out;
    // Add left to create (left + (~right) + 1) = left - right
    out[i] = left[i] + temp[i];
    carry_out = (out[i] < left[i] || out[i] < temp[i]) ? 1 : 0;
  }

  memcpy(left, out, sizeof(out));
}

static int u32_lesser_than(u32 *left, u32 *right) {
  for (int i = 0; i < 2 * SIZE; i++) {
    if (left[i] < right[i])
      return TRUE;
    if (left[i] > right[i])
      return FALSE;
  }
  return FALSE;  // Equal
}

int divide(number_t *out, number_t *denominator, number_t *numerator, int wordsize) {
  number_t quotient = {0};
  number_t remainder = {0};
  if (ERROR == div_and_mod(&quotient, &remainder, denominator, numerator, wordsize))
    return ERROR;
  *out = quotient;
  return SUCCESS;
}

int modulo(number_t *out, number_t *denominator, number_t *numerator, int wordsize) {
  number_t quotient = {0};
  number_t remainder = {0};
  if (ERROR == div_and_mod(&quotient, &remainder, denominator, numerator, wordsize))
    return ERROR;
  *out = remainder;
  return SUCCESS;
}

int lshift(number_t *out, number_t *number, number_t *positions, int wordsize) {
  number_t stk_num = *number;
  zero_number(out);
  bubble_up_metadata(out, &stk_num, positions);
  // check for unsigned overflows
  u64 lshift = positions->num[SIZE - 1];
  number_t max_num = ZERO(wordsize), shift_max_num = ZERO(wordsize);
  get_max_number(&max_num, wordsize);
  rshift(&shift_max_num, &max_num, positions, wordsize);
  if (greater_than(&stk_num, &shift_max_num)) {
    out->metadata.UNSIGNED_OVERFLOW = 1;
  }
  out->wordsize = wordsize;
  u64 shift_in = 0;
  for (int i = SIZE - 1; i >= (int)(0 + lshift / WIDTH); i--) {
    out->num[i - (int)lshift / WIDTH] =
        (stk_num.num[i] << lshift % WIDTH) |
        (shift_in >> ((WIDTH - lshift) % WIDTH));
    shift_in = (((1ULL << lshift) - 1) << (WIDTH - lshift)) & stk_num.num[i];
  }
  return SUCCESS;
}

int rshift(number_t *out, number_t *number, number_t *positions, int wordsize) {
  // make sure to zero out the most significant bits
  number_t stk_num = *number;
  number_t mask = {0};
  get_max_number(&mask, wordsize);
  and(&stk_num, number, &mask, wordsize);
  zero_number(out);
  bubble_up_metadata(out, &stk_num, positions);
  u64 rshift = positions->num[SIZE - 1];
  if (rshift >= wordsize || rshift >= SIZE * WIDTH) {
#ifdef DEBUG
    printf("%s: shifting_value=0x%llx is greater than max number of bits\n",
           __FUNCTION__, rshift);
#endif
    memset(out->num, 0, SIZE_BYTES);
    return SUCCESS;
  }
  out->wordsize = wordsize;
  u64 shift_in = 0;
  for (int i = 0; i < SIZE - rshift / WIDTH; i++) {
    out->num[i + rshift / WIDTH] =
        stk_num.num[i] >> rshift | shift_in << (WIDTH - rshift);
    shift_in = stk_num.num[i] & ((1ULL << rshift % WIDTH) - 1);
  }
  return SUCCESS;
}

/*         compare        */
static int compare(const number_t *a, const number_t *b) {
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

int greater_than(const number_t *a, const number_t *b) {
  return 1 == compare(a, b);
}

int equal_to(const number_t *a, const number_t *b) {
  return 0 == compare(a, b);
}

int lesser_than(const number_t *a, const number_t *b) {
  return -1 == compare(a, b);
}

static int zero_number(number_t *out) {
  if (!out)
    return ERROR;
  for (int i = 0; i < SIZE; i++) {
    out->num[i] = 0;
  }
  return SUCCESS;
}

/*           and             */
int and(number_t *out, number_t *a, number_t *b, int wordsize) {
  if (out == NULL || a == NULL || b == NULL)
    return ERROR;
  bubble_up_metadata(out, a, b);
  out->wordsize = wordsize;
  for (int i = 0; i < SIZE; i++)
    out->num[i] = a->num[i] & b->num[i];
  return SUCCESS;
}

/*             or              */
int or(number_t *out, number_t *a, number_t *b, int wordsize) {
  if (out == NULL || a == NULL || b == NULL)
    return ERROR;
  bubble_up_metadata(out, a, b);
  for (int i = 0; i < SIZE; i++)
    out->num[i] = a->num[i] | b->num[i];
  return SUCCESS;
}

static int get_max_number(number_t *out, int wordsize) {
  out->wordsize = wordsize;
  int current_ws = wordsize;
  int n_ptr = SIZE - 1;
  while (current_ws > 0) {
    out->num[n_ptr--] =
        current_ws >= WIDTH ? UINT64_MAX : (1ULL << current_ws) - 1;
    current_ws -= WIDTH;
  }
  return SUCCESS;
}

void print_unsigned_decimal(number_t *number) {
  print_decimal(number, 0);
}
  
void print_signed_decimal(number_t *number) {
  int ws = number->wordsize;
  // Create a mask for the number's wordsize and apply it
  number_t mask = ZERO(ws);
  get_max_number(&mask, ws);
  number_t masked_number = ZERO(ws);
  and(&masked_number, number, &mask, ws);
  
  if (ws == 1) {
    if (compare(number, &_zero_))
      printf("0");
    else
      printf("1");
  } else if (((1ULL << (ws - 1) % WIDTH) & masked_number.num[SIZE - ws / (WIDTH+1) - 1]) > 0) {
    // // negative number
    number_t complement = ZERO(ws);
    twos_comp(&complement, &masked_number, ws);
    printf("-");
    print_unsigned_decimal(&complement);
  } else {
    // positive number
    print_unsigned_decimal(number);
  }
}

static void print_decimal(number_t *number, int is_signed) {
  
  // Create a mask for the number's wordsize and apply it
  number_t mask = ZERO(number->wordsize);
  get_max_number(&mask, number->wordsize);
  number_t masked_number = ZERO(number->wordsize);
  and(&masked_number, number, &mask, number->wordsize);

  if (greater_than(&masked_number, &MAX_DECIMAL)) {
    printf("Too large to represent.");
    return;
  }
  number_t scratch = ZERO(SIZE * WIDTH);
  number_t cloned = masked_number;

  if (0 == compare(&cloned, &_zero_)) {
    printf("0");
    fflush(stdout);
    return;
  }
  number_t six_four = {8, {0, 64}, {0}};
  cloned.wordsize = SIZE * WIDTH;
  int n_chunks = SIZE;
  while (cloned.num[0] == 0) {
    lshift(&cloned, &cloned, &six_four, cloned.wordsize);
    n_chunks--;
  }
  u64 bits, new_chunk;

  for (int i = 0; i < n_chunks * WIDTH; i++) {
    lshift(&scratch, &scratch, &_one_, scratch.wordsize);

    if ((cloned.num[0] & (1ULL << 63)) > 0) {
      scratch.num[SIZE - 1] |= 1;
    }

    if (i == (n_chunks * 64) - 1)
      break;

    for (int j = SIZE - 1; j >= 0; j--) {
      if (scratch.num[j] == 0)
        continue;
      new_chunk = 0;
      for (int k = 60; k >= 0; k -= 4) {
        bits = (scratch.num[j] & (0b1111ULL << k)) >> k;
        if (bits >= 5)
          bits += 3ULL;
        new_chunk <<= 4;
        new_chunk += bits;
      }
      scratch.num[j] = new_chunk;
    }
    lshift(&cloned, &cloned, &_one_, cloned.wordsize);
  }
#ifdef DEBUG
#endif
  int start = 0;
  for (int i = 0; i < SIZE; i++) {
    if (scratch.num[i] == 0)
      continue;
    for (int j = 60; j >= 0; j -= 4) {
      bits = (scratch.num[i] & (0xfULL << j)) >> j;
      if (!start && bits == 0)
        continue;
      if (!start)
        start = 1;
      printf("%llu", bits);
    }
  }
}

/***********************************/
/*********       TESTS     *********/
/***********************************/

#ifdef UNIT_TEST

int construct_number(number_t *out, int wordsize, u64 msb, u64 lsb) {
  number_t constructed = { wordsize, {msb, lsb}, {0}};
  *out = constructed;
  return SUCCESS;
}

int numbers_equal(number_t *test, u64 correct_h, u64 correct_l) {
  return test->num[0] == correct_h && test->num[1] == correct_l;
}

int numbers_equal_metadata(number_t *test, u64 correct_h, u64 correct_l, unsigned short correct_bm) {
  return test->num[0] == correct_h 
    && test->num[1] == correct_l
    && (test->metadata.SIGNED_OVERFLOW<<2 | test->metadata.UNSIGNED_OVERFLOW<<1 | test->metadata.INTERPRET_SIGNED ) == correct_bm
    ;
}

int isEqualToBitstring(number_t *n, char *s) {
  if (!n || !s) {
    printf("%s: either number or bitstring was null\n", __FUNCTION__);
    return 1;
  }
  int len = strlen(s);

  for (int i = 1; i <= len; i++) {
    if ((n->num[SIZE - (n->wordsize - i - 1) / WIDTH - 1] &
         (1ULL << (i - 1) % WIDTH)) >>
            (i - 1) % WIDTH !=
        s[len - i] - '0') {
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
  print_bitstring(n.num, wordsize);
  printf("\n");
  printf("expected two's complement = %s\n", expected);
  twos_comp(&twos, &n, wordsize);
  printf("actual two's complement = ");
  print_bitstring(twos.num, wordsize);
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
  print_bitstring(n.num, wordsize);
  printf("\n");
  printf("pos = ");
  print_bitstring(p.num, wordsize);
  printf("\n");
  printf("expected num << pos = %s\n", expected);
  lshift(&res, &n, &p, wordsize);
  printf("actual num << pos = ");
  print_bitstring(res.num, wordsize);
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
  print_bitstring(n.num, wordsize);
  printf("\n");
  printf("pos = ");
  print_bitstring(p.num, wordsize);
  printf("\n");
  printf("expected num >> pos = %s\n", expected);
  rshift(&res, &n, &p, wordsize);
  printf("actual num >> pos = ");
  print_bitstring(res.num, wordsize);
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
  print_bitstring(a.num, aWs);
  printf("\n");
  printf("b = ");
  print_bitstring(b.num, bWs);
  printf("\n");
  printf("expected a+b = %s\n", expected);
  add(&sum, &a, &b, oWs);
  printf("actual a+b = ");
  print_bitstring(sum.num, oWs);
  printf("\n");
  int ret = isEqualToBitstring(&sum, expected);
  if (!ret)
    printf("Test Passed!\n");
  else
    printf("Test Failed!\n");
  return ret;
}

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
  print_bitstring(b.num, bWs);
  printf("\n");
  printf("a = ");
  print_bitstring(a.num, aWs);
  printf("\n");
  printf("expected b-a = %s\n", expected);
  sub(&sum, &a, &b, oWs);
  printf("actual b-a = ");
  print_bitstring(sum.num, oWs);
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
  number_t a = {0}, b = {0}, anded = {0};
  new_number(&a, BINARY, aS, aWs);
  new_number(&b, BINARY, bS, bWs);
  printf("b = ");
  print_bitstring(b.num, bWs);
  printf("\n");
  printf("a = ");
  print_bitstring(a.num, aWs);
  printf("\n");
  printf("expected a&b = %s\n", expected);
  and(&anded, &a, &b, ws);
  number_print(&anded);
  printf("actual a&b = ");
  print_bitstring(anded.num, ws);
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
  print_bitstring(a.num, aWs);
  printf("\n");
  number_print(&a);
  printf("b = ");
  print_bitstring(b.num, bWs);
  printf("\n");
  number_print(&b);
  printf("expected a|b = %s\n", expected);
  or(&ored, &a, &b, ws);
  printf("actual a|b = ");
  print_bitstring(ored.num, ws);
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
