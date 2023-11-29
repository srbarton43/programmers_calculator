/*
 * common utils header
 */
#ifndef __UTILS_H
#define __UTILS_H

#include "hashtable.h"

#define HT_SIZE 103  // the default hashtable size
#define DEFAULT_WS 8 // the wordsize when pcalc starts
#define VAR_NUM 26   // the number of vars (26 for each lcase char in alphabet)

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

// initializes program data values
program_data_t* init_program_data(void);

// prints program data for debugging
void print_program_data(program_data_t* prog_data);

// frees program data struct
void free_program_data(program_data_t* prog_data);

#endif
