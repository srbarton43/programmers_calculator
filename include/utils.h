/*
 * common utils header
 */
#ifndef __UTILS_H
#define __UTILS_H

#include "hashtable.h"
#include "number.h"

#define nums_SIZE 103  // the default hashtable size
#define DEFAULT_WS 8 // the wordsize when pcalc starts
#define VAR_NUM 26   // the number of vars (26 for each lcase char in alphabet)

#define SUCCESS 0
#define ERROR (-1)

typedef struct program_data {
  hashtable_t* nums;
  int wordsize;
  char* vars[VAR_NUM];
} program_data_t;

// program data struct ... holds all program state
extern program_data_t* prog_data;

// return max(a,b)
int max(int a, int b);

// return min(a,b)
int min(int a, int b);

// PROGRAM DATA FUNCTIONS
// initializes program data values
program_data_t* init_program_data(void);

// prints program data for debugging
void print_program_data(program_data_t* prog_data);

// frees program data struct
void free_program_data(program_data_t* prog_data);

// NUM HASHTABLE FUNCTIONS

/*
 * nums_add_string - adds bitstring->number pair to ht
 * 
 * params:
 *    const char* number  := string representing number
 *    type_e type         := enum type of number 
 * returns:
 *    char* key           := bitstring used as key in ht
 *    NULL                := if error
 * 
 * caller must:
 *    free return val at some point
 */
char* nums_add_string(program_data_t* prog_data, const char* number, type_e type);

/*
 * nums_add_number - adds bitstring->number pair to ht
 * 
 * params:
 *    number_t* number  := number struct
 * returns:
 *    char* key         := bitstring used as key in ht
 * 
 * caller must:
 *    free return val at some point
 */
char* nums_add_number(program_data_t* prog_data, number_t* number);

/*
 * nums_get_num - gets number from hashtable
 * 
 * params:
 *    const char* number  := bitstring as key
 * returns:
 *    number_t* num       := pointer to the number
 */
number_t* nums_get_num(program_data_t* prog_data, const char* number);

char* vars_get_val(program_data_t* prog_data, char var);

void vars_set_val(program_data_t* prog_data, char var, char* val);

#endif
