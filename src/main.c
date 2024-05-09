#include "stdio.h"

#include "number.h"
#include "parser.tab.h"
#include "utils.h"

void yylex_destroy(void);
typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

// extern hashtable_t* ht;
// extern int global_wordsize;
program_data_t *prog_data;

int main() {
  prog_data = init_program_data();
  char string[] = "1+4\n";
  YY_BUFFER_STATE buffer = yy_scan_string(string);
  yyparse();
  yy_delete_buffer(buffer);
  yylex_destroy();
  free_program_data(prog_data);
  return 0;
//  prog_data = init_program_data();
//  printf(">>> ");
//  int ret = yyparse();
//  yylex_destroy();
//  printf("\nThanks for using pcalc :)\n");
//#ifdef DEBUG
//  print_program_data(prog_data);
//#endif
//  free_program_data(prog_data);
//  return ret;
}
