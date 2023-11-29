#include <stdlib.h>
#include "utils.h"
#include "hashtable.h"
#include "number.h"

static hashtable_t* setup_ht();
// function wrappers for hashtable function pointers
static void item_delete(void* item);
static void item_print(FILE* fp, const char* key, void* item);

int max (int a, int b) {
  if (a >= b)
    return a;
  return b;
}

int min (int a, int b) {
  if (a <= b) 
    return a;
  return b;
}

program_data_t* init_program_data(void) {
  program_data_t* p_data = malloc(sizeof(program_data_t));

  p_data->nums = setup_ht();
  p_data->wordsize = DEFAULT_WS;

  return p_data;
}

void print_program_data(program_data_t* p_data) {
  printf("\n***********************\n"); printf("Numbers Table\n");
  hashtable_print(p_data->nums, stdout, item_print);
  printf("wordsize: %d\n", p_data->wordsize);
}

void free_program_data(program_data_t* prog_data) {
  hashtable_delete(prog_data->nums, item_delete);
  free(prog_data);
}

static hashtable_t* setup_ht() {
  hashtable_t* ht = hashtable_new(HT_SIZE);
  hashtable_insert(ht, "0", copy_number(_zero_, 0));
  // hashtable_insert(ht, "1", copy_number(_one_, 0));
  return ht;
}

static void item_delete(void* item) {
  delete_number((number_t*)item);
}

static void item_print(FILE* fp, const char* key, void* item) {
  fprintf(fp, "key: %s\n", key);
  number_print((number_t*)item);
}
