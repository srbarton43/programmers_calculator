/*
 * common utils header
 */
#ifndef __UTILS_H
#define __UTILS_H

#include "number.h"

#define nums_SIZE 103 // the default hashtable size
#define DEFAULT_WS 8  // the wordsize when pcalc starts
#define VAR_NUM 26    // the number of vars (26 for each lcase char in alphabet)
#define MAX_NUMBERS_COUNT 32

struct status_bitfield {
  unsigned int NUM_BUF_OF : 1; // more numbers than buffer can handle
  unsigned int POISON : 1;     // not sure what this means
  unsigned int VAR_ASSN : 1;   // high if there was a variable assignment
  unsigned int WSIZE_CHG : 1;  // high if wordsize change
};

typedef struct program_data {
  int wordsize;
  struct status_bitfield status;
  number_t vars[VAR_NUM];
  number_t numbers_buf[MAX_NUMBERS_COUNT];
  int nbuf_ptr;
} program_data_t;

// program data struct ... holds all program state
extern program_data_t *prog_data;

// return max(a,b)
int max(int a, int b);

// return min(a,b)
int min(int a, int b);

// PROGRAM DATA FUNCTIONS
// initializes program data values
program_data_t *init_program_data(void);

// prints program data for debugging
void print_program_data(program_data_t *prog_data);

// frees program data struct
void free_program_data(program_data_t *prog_data);

number_t vars_get_num(program_data_t *prog_data, char var);

void vars_set_num(program_data_t *prog_data, char var, number_t *num);

#endif
