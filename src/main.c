
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

#include "parser.tab.h"
#include "number.h"
void yylex_destroy(void);
void item_delete(void* item);
void item_print(FILE* fp, const char* key, void* item);

extern hashtable_t* ht;

int main() {
  ht = hashtable_new(10);
  int ret = yyparse();
  yylex_destroy();
  printf("\n****** printing hashtable ******\n");
  hashtable_print(ht, stdout, item_print);
  hashtable_delete(ht, item_delete);
  return ret;
}

void item_delete(void* item) {
  delete_number((number_t*)item);
}

void item_print(FILE* fp, const char* key, void* item) {
  fprintf(fp, "key: %s\n", key);
  number_print((number_t*)item);
}
