
#ifndef __NUMBER_H
#define __NUMBER_H

#include <stdint.h>

#define SUCCESS 0
#define ERROR (-1)

// size for num rep. arr.
#define SIZE 2

typedef uint64_t u64;
typedef uint8_t u8;

typedef enum { BINARY, DECIMAL, HEXADECIMAL } type_e;


// Complete definition lives in number.c
typedef struct number number_t;

/************** FUNCTIONS *******************/

/************** n_SIZE_OF ******************/
uint32_t n_sizeof(void);

int number_alloc(number_t **out);
int number_destroy(number_t *num);
int number_zero(number_t *num);
unsigned short n_UNSIGNED_OVERFLOW(number_t *num);
unsigned short n_SIGNED_OVERFLOW(number_t *num);
u64 n_getLeastSigChunk(number_t *out);

/************** NEW_NUMBER ******************/
/* Create a New Number
 *
 * params:
 *    number_t out  := pointer to new number struct as output
 *    type_e type   := enum type of the number_string passed
 *    char* number  := null-terminated string for number
 *    int wordsize  := wordsize for number (max bits allocated)
 *
 * returns:
 *    SUCCESS on success
 *    ERROR on error
 *
 * caller may:
 *    free param number
 *
 * example: new_number(&number, BINARY, "1101", 9)
 */
int new_number(number_t *out, type_e type, const char *number, int wordsize);

/************** NUMBER_PRINT ****************/
/* Prints the Number Struct to stdout
 *
 * params:
 *    number_t* number  := valid pointer to number_t
 *
 * returns:
 *    nothing
 *
 * example: number_print(num)
 */
void number_print(number_t *number);

/************** DELETE_NUMBER ****************/
/* Frees the Number Struct From Heap
 *
 * params:
 *    number_t* number := valid pointer to allocated number struct
 *
 * returns:
 *    nothing
 *
 * callee guarantees:
 *    number will be freed from heap
 */
void delete_number(number_t *number);

void print_signed_decimal(number_t *number);

void print_unsigned_decimal(number_t *number);

/*
 * copies number param
 *
 * params:
 *    number_t* number  := number to copy
 *    int wordsize      := wordsize of returned number (same as param if 0)
 * returns:
 *    allocated number_t* num where
 *      num->wordsize = number->wordsize ? wordsize == 0
 *        else num->wordsize = wordsize
 *      number->bits = num->bits
 *
 * caller must:
 *    call delete_number(num)
 *
 * we guarantee:
 *    params aren't modified
 */
int copy_number(number_t *new, number_t *old, int wordsize);

/**********     FREE_NUMBERS     ********/
/*
 * Frees special numbers
 */
void free_numbers(void);

/**********     number_getSdec       *******/
/*
 * returns signed integer rep for number
 *
 * params:
 *    number_t* number  := number
 *
 * returns:
 *    signed int := signed integer rep for param number
 *
 * we guarantee:
 *    number is unchanged
 */
int number_getSdec(int64_t *out, number_t *number);

/*
 * number_getHex
 *
 * params:
 *    number_t* number  := pointer to number
 *
 * return:
 *    char* hex := alloc'ed hexstring
 *
 * caller must:
 *    free the returned hex string
 */
char *number_getHex(number_t *number);

/////////////////////////////////////////
//           OPERATORS                 //
/////////////////////////////////////////

/********* ONE's COMPLEMENT****************/
/*
 * returns one's complement of num (bitwise not)
 *
 * params:
 *    number_t *out := pointer to result number_t
 *    number_t* num := valid pointer to number 
 *    int wordsize  := wordsize for
 *
 * returns:
 *    SUCCESS on success
 *    ERROR on failure
 */
int ones_comp(number_t *out, number_t *num, int wordsize);

/********* TWO's COMPLEMENT****************/
/*
 * returns two's complement of num (integer negation)
 *
 * params:
 *    number_t* out := pointer to output number
 *    number_t* num := valid pointer to number
 *    int wordsize  := wordsize for return number
 *
 * returns:
 *    SUCCESS on success
 *    ERROR on failure
 */
int twos_comp(number_t *out, number_t *num, int wordsize);

// returns 1 if a > b
// zero otherwise
int greater_than(const number_t *a, const number_t *b);

int lesser_than(const number_t *a, const number_t *b);

int equal_to(const number_t *a, const number_t *b);

/************** ADD ***********************/
/*
 * adds two numbers together
 *
 * params:
 *    number_t *out := pointer to output number
 *    number_t* a   := pointer to number
 *    number_t* b   := pointer to number
 *    int wordsize  := wordsize for output number
 *
 * returns:
 *    SUCCESS on success
 *    ERROR on failure
 *
 */
int add(number_t *out, number_t *a, number_t *b, int wordsize);

/***************** SUB ***********************/
/*
 * subtract a from b
 */
int sub(number_t *out, number_t *a, number_t *b, int wordsize);

/***************** DIVIDE and MOD ***********************/
int divide(number_t *out, number_t *denominator, number_t *numerator, int wordsize);
int modulo(number_t *out, number_t *denominator, number_t *numerator, int wordsize);

int multiply(number_t *out, number_t *a, number_t *b, int wordsize);

/************** LSHIFT ***********************/
/*
 * lshifts number by positions
 *
 * params:
 *    number_t *out       := pointer to output number
 *    number_t* number    := pointer to number to be shifted
 *    number_t* positions := how many positions to shift
 *
 * returns:
 *    SUCCESS on success
 *    ERROR on failure
 *
 */
int lshift(number_t *out, number_t *number, number_t *positions, int wordsize);

/************** RSHIFT ***********************/
/*
 * rshifts number by positions
 *
 * params:
 *    number_t *out       := pointer to output number
 *    number_t* number    := pointer to number to be shifted
 *    number_t* positions := pointer to number holding shift amount
 *
 * returns:
 *    SUCCESS on success
 *    ERROR on failure
 */
int rshift(number_t *out, number_t *number, number_t *positions, int wordsize);

/*
 * bitwise and two numbers together
 *
 * params:
 *    number_t *out   := pointer to output number
 *    number_t* a     := pointer to first number
 *    number_t* b     := pointer to second number
 *
 * returns:
 *    SUCCESS on success
 *    ERROR on failure
 */
int and(number_t *out, number_t *a, number_t *b, int wordsize);

/*
 * bitwise or two numbers together
 *
 * params:
 *    number_t *out   := pointer to output number
 *    number_t* a     := pointer to first number
 *    number_t* b     := pointer to second number
 *
 * returns:
 *    SUCCESS on success
 *    ERROR on failure
 */
int or(number_t *out, number_t *a, number_t *b, int wordsize);

#ifdef DEBUG
int number_debug(number_t *num);
#endif

#ifdef UNIT_TEST

int numbers_equal(number_t *test, u64 correct_h, u64 correct_l);
int numbers_equal_metadata(number_t *test, u64 correct_h, u64 correct_l, unsigned short correct_bm);
int construct_number(number_t *out, int wordsize, u64 msb, u64 lsb);
int test_twos_comp(char *num, char *expected, int wordsize, char *msg);
int test_add(char *aS, int aWs, char *bS, int bWs, int oWs, char *expected, char *msg);
int test_sub(char *aS, int aWs, char *bS, int bWs, int oWs, char *expected, char *msg);
int test_copy_number(char *num, int iws, int ows, char *expected, char *msg);
int test_lshift(char *num, char *pos, char *expected, int wordsize, char *msg);
int test_rshift(char *num, char *pos, char *expected, int wordsize, char *msg);
int test_and(int ws, char *a, int aWs, char *b, int bWs, char *expected,
             char *msg);
int test_or(int ws, char *a, int aWs, char *b, int bWs, char *expected,
            char *msg);
#endif

#endif // NUMBER_H
