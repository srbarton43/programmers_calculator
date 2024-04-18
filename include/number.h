
#ifndef __NUMBER_H
#define __NUMBER_H

typedef enum {BINARY, DECIMAL, HEXADECIMAL} type_e;

typedef struct number {
  int wordsize;   // max wordsize for the bitstring
  int len;        // actual length of bitstring
  char* bits;     // non null-terminated bitstring
} number_t;      

extern number_t* _zero_;
extern number_t* _one_;

/************** FUNCTIONS *******************/

/************** NEW_NUMBER ******************/
/* Create a New Number
 * 
 * params:
 *    type_e type   := enum type of the number_string passed
 *    char* number  := null-terminated string for number
 *    int wordsize  := wordsize for number (max bits allocated)
 *
 * returns:
 *    allocated number_t struct with fields defined
 *    OR NULL if an error
 *
 * caller must:
 *    call delete_number to free the number on the heap
 *
 * caller may:
 *    free param number
 *
 * example: new_number(BINARY, "1101", 9)
 */
number_t* new_number(type_e type, const char* number, int wordsize);

/********       CHANGE_WORDSIZE      ********/
/*
 * changes wordsize of number to new wordsize
 * 
 * params:
 *    number_t* num := number you want to change wordsize
 *    int wordsize  := new wordsize
 * 
 * returns:
 *    nothing
 * 
 * we guarantee:
 *    num->wordsize = wordsize
 *    num->bits is left-padded with '0's if new wordsize is bigger
 *              and most-sig bits chopped off if wordsize is smaller
 */
void change_wordsize(number_t* num, int wordsize);

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
void number_print(number_t* number);

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
void delete_number(number_t* number);

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
number_t* copy_number(number_t* number, int wordsize);

int numbers_are_equal(number_t* a, number_t* b);

/**********     INIT_NUMBERS     ********/
/*
 * Initializes special numbers
 */
void init_numbers(void);

/**********     FREE_NUMBERS     ********/
/*
 * Frees special numbers
 */
void free_numbers(void);

/**********     number_getUdec       *******/
/*
 * returns unsigned integer rep for number
 *
 * params:
 *    number_t* number  := number
 * 
 * returns:
 *    unsigned int := unsigned integer rep for param number
 * 
 * we guarantee:
 *    number is unchanged
 */
unsigned long long number_getUdec(number_t* number);

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
signed long long number_getSdec(number_t* number);

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
char* number_getHex(number_t* number);

/////////////////////////////////////////
//           OPERATORS                 //
/////////////////////////////////////////

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
void dec2binary(unsigned long long decimal, char* binary, int wordsize);

/*             hex2binary               */
/*
 * Converts hexstring to bitstring
 * 
 * params:
 *    char* hex     := hexstring (not including '0x')
 *    char* binary  := bitstring
 * 
 * returns:
 *    nothing
 */
void hex2binary(const char* hex, char* binary, int wordsize);

/********* ONE's COMPLEMENT****************/
/*
 * returns one's complement of num (bitwise not)
 *
 * params:
 *    number_t* num := valid pointer to number int wordsize  := wordsize for return number returns:
 *    number_t* number where...
 *      number->wordsize = wordsize
 */
number_t* ones_comp(number_t* num, int wordsize);

/********* TWO's COMPLEMENT****************/
/*
 * returns two's complement of num (integer negation)
 *
 * params:
 *    number_t* num := valid pointer to number
 *    int wordsize  := wordsize for return number
 *
 * returns:
 *    number_t* number where...
 *      number->wordsize = wordsize
 */
number_t* twos_comp(number_t* num, int wordsize);

/************** ADD ***********************/
/*
 * adds two numbers together
 *
 * params:
 *    number_t* a := pointer to number
 *    number_t* b := pointer to number
 *    (a and b must have same wordsize)
 *
 * returns:
 *    pointer to number_t on heap which is sum (wordsize = a&b->ws)
 *    OR NULL if error
 *
 * caller must:
 *    call delete_number on all params and return val
 *    
 * callee guarantees:
 *    paramaters are unchanged
 */
number_t* add (number_t* a, number_t* b, int wordsize);

/***************** SUB ***********************/
/*
 * subtract a from b
 */
number_t* sub (number_t* a, number_t* b, int wordsize);

/************** LSHIFT ***********************/
/*
 * lshifts number by positions
 *
 * params:
 *    number_t* number := pointer to number
 *    number_t* positions := pointer to number
 *
 * returns:
 *    pointer to number_t on heap 
 *    OR NULL if error
 *
 * caller must:
 *    call delete_number on all params and return val
 *    
 * callee guarantees:
 *    paramaters are unchanged
 */
number_t* lshift (number_t* number, number_t* positions, int wordsize);

/************** RSHIFT ***********************/
/*
 * rshifts number by positions
 *
 * params:
 *    number_t* number := pointer to number
 *    number_t* positions := pointer to number
 *
 * returns:
 *    pointer to number_t on heap 
 *    OR NULL if error
 *
 * caller must:
 *    call delete_number on all params and return val
 *    
 * callee guarantees:
 *    paramaters are unchanged
 */
number_t* rshift (number_t* number, number_t* positions, int wordsize);

/*
 *
 */
number_t* and(number_t* a, number_t* b, int wordsize);

/*
 *
 */
number_t* or(number_t* a, number_t* b, int wordsize);


#ifdef UNIT_TEST

int test_twos_comp(char* num, char* expected, int wordsize, char* msg);
int test_add (char* aS, int aWs, char* bS, int bWs, char* expected, char*  msg);
int test_sub (char* aS, int aWs, char* bS, int bWs, char* expected, char*  msg);
int test_copy_number (char* num, int iws, int ows, char* expected, char* msg);
int test_lshift(char* num, char* pos, char* expected, int wordsize, char* msg);
int test_rshift(char* num, char* pos, char* expected, int wordsize, char* msg);
int test_and(int ws, char* a, int aWs, char* b, int bWs, char* expected, char* msg);
int test_or(int ws, char* a, int aWs, char* b, int bWs, char* expected, char* msg);

int test_dec2binary(unsigned long long decimal, char* expected, int wordsize);
int test_hex2binary(char* hex, char* expected);

#endif

#endif // NUMBER_H
