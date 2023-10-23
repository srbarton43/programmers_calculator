
#ifndef __NUMBER_H
#define __NUMBER_H

#include <stdio.h>

enum type {BINARY, DECIMAL, HEX};
typedef struct number number_t; // number struct

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

#endif // NUMBER_H
