
#ifndef __NUMBER_H
#define __NUMBER_H

#include <stdio.h>
#include <stdbool.h>

enum type {BINARY, DECIMAL, HEX};

typedef struct number {
  int wordsize;
  int len;
  bool isSign;
  char* bits;
} number_t; // number struct

/************** FUNCTIONS *******************/

/************** NEW_NUMBER ******************/
/* Create a New Number
 * 
 */
number_t* new_number(int type, char* number);

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

#endif // NUMBER_H
