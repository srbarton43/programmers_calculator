
#ifndef __NUMBER_H
#define __NUMBER_H

#include <stdio.h>
#include <stdbool.h>

typedef enum {BINARY, DECIMAL, HEX} type_e;

typedef struct number {
  int wordsize;   // max wordsize for the bitstring
  int len;        // actual length of bitstring
  bool isSign;    // is it signed binary?
  char* bits;     // non null-terminated bitstring
} number_t;      

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
number_t* new_number(type_e type, char* number, int wordsize);

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
number_t* add (number_t* a, number_t* b);

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
number_t* lshift (number_t* number, number_t* positions);

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
number_t* rshift (number_t* number, number_t* positions);


#ifdef UNIT_TEST
int test_add(char* a, char* b, char* expected, int wordsize, char* msg);
int test_lshift(char* num, char* pos, char* expected, int wordsize, char* msg);
#endif

#endif // NUMBER_H
