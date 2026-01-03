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
#define PROMPT ">>> "

typedef struct status_bitfield {
  unsigned short POISON : 1;     // not sure what this means
  unsigned short VAR_ASSN : 1;   // high if there was a variable assignment
  unsigned short WSIZE_CHG : 1;  // high if wordsize change
  unsigned short WSIZE_PR : 1;  // high if wordsize print
  unsigned short QUIT_SIG: 1;   // high if quit is asserted
  unsigned short EMPTY : 1;     // high if line was empty
  unsigned short UNDEF_VAR : 1;     // high if unset variable was used
  unsigned short ACCESS_VAR : 1; // high if var is accessed
} status_t;

typedef struct number_flag {
  number_t *number;
  enum {IS_VAR, NOT_VAR} flag;
} number_flag_t;

typedef struct program_data {
  unsigned int wordsize;
  struct status_bitfield status; // to remove
  number_t *vars[VAR_NUM];
} program_data_t;

typedef struct print_flags {
  unsigned short VERBOSE  : 1;
  unsigned short BINARY   : 1;
  unsigned short HEX      : 1;
  unsigned short DECIMAL  : 1;
  unsigned short UNSIGNED : 1;
} print_flags_t;


// program data struct ... holds all program state
extern program_data_t g_prog_data;

int el_mainloop(print_flags_t print_flags);

int rl_mainloop(void);

int evaluate_expr(const char *expr, print_flags_t print_flags);

// PROGRAM DATA FUNCTIONS
// prints program data for debugging
void print_program_data(program_data_t *prog_data);

int vars_get_num(number_t **out, char var);

void vars_set_num(char var, number_t *num);

#endif
