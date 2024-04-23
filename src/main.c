#include "number.h"
#include "parser.tab.h"
#include "utils.h"

void yylex_destroy(void);

// extern hashtable_t* ht;
// extern int global_wordsize;
program_data_t *prog_data;

int main() {
  init_numbers();
  prog_data = init_program_data();
  printf(">>> ");
  int ret = yyparse();
  yylex_destroy();
  printf("\nThanks for using pcalc :)\n");
#ifdef DEBUG
  print_program_data(prog_data);
#endif
  free_program_data(prog_data);
  free_numbers();
  return ret;
}
