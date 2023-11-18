
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
 */
number_t* new_number(type_e type, char* number, int wordsize);

/************** NUMBER_PRINT ****************/
/* Prints the Number Struct
 *
 */
void number_print(number_t* number);

/************** DELETE_NUMBER ****************/
/* Frees the Number Struct From Heap
 *
 */
void delete_number(number_t* number);

/************** LSHIFT ***********************/
number_t* lshift (number_t* number, int positions);

/************** RSHIFT ***********************/
number_t* rshift (number_t* number, int positions);

#ifdef UNIT_TEST
int test_add(char* a, char* b, char* expected, int wordsize, char* msg);
#endif

#endif // NUMBER_H
