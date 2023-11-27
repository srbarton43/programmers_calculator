
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"
#include "number.h"
void yylex_destroy(void);

int main() {
  int ret = yyparse();
  yylex_destroy();
  return ret;
    
}
