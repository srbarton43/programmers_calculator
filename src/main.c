
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"
#include "number.h"
void yylex_destroy(void);

int main(int argc, char *argv[]) {
  int ret = yyparse();
  yylex_destroy();
  number_t* num = new_number(BINARY, "1101", 8);
  number_print(num);
  delete_number(num);
  return ret;
    
}
