#include <stdio.h>
#include <stdlib.h>

#include "number.h"
#include "utils.h"

static const struct status_bitfield _empty_bitfield_;

int max(int a, int b) {
  if (a >= b)
    return a;
  return b;
}

int min(int a, int b) {
  if (a <= b)
    return a;
  return b;
}

program_data_t *init_program_data(void) {
  program_data_t *p_data = malloc(sizeof(program_data_t));

  for (int i = 0; i < VAR_NUM; i++) {
    copy_number(&p_data->vars[i], &_zero_, 4);
  }
  p_data->nbuf_ptr = 0;
  p_data->wordsize = DEFAULT_WS;
  p_data->status = _empty_bitfield_;

  return p_data;
}

void print_program_data(program_data_t *p_data) {
  printf("\n***********************\n");
  printf("Variables\n");
  for (int i = 0; i < VAR_NUM; i++) {
    printf("%c = ", 'a' + i);
    number_print(&p_data->vars[i]);
  }
  printf("wordsize: %d\n", p_data->wordsize);
}

void free_program_data(program_data_t *p_data) { free(p_data); }

number_t vars_get_num(program_data_t *prog_data, char var) {
  int idx = var - 'a';
  number_t num = prog_data->vars[idx];
  return num;
}

void vars_set_num(program_data_t *prog_data, char var, number_t *num) {
  int idx = var - 'a';
  copy_number(&prog_data->vars[idx], num, num->wordsize);
}
