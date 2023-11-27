#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

#include "parser.tab.h"
#include "number.h"
#include "utils.h"

void yylex_destroy(void);
// function wrappers for hashtable function pointers
void item_delete(void* item);
void item_print(FILE* fp, const char* key, void* item);

extern hashtable_t* ht;

int main() {
  init_numbers();
  ht = setup_ht();
  int ret = yyparse();
  yylex_destroy();
  printf("********************************\n");
  printf("****** printing hashtable ******\n");
  hashtable_print(ht, stdout, item_print);
  hashtable_delete(ht, item_delete);
  free_numbers();
  return ret;
}

void item_delete(void* item) {
  delete_number((number_t*)item);
}

void item_print(FILE* fp, const char* key, void* item) {
  fprintf(fp, "key: %s\n", key);
  number_print((number_t*)item);
}
