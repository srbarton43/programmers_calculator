#include <stdlib.h>
#include <string.h>
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
  p_data->ten_bit_nums = setup_ht();
  for (int i = 0; i < VAR_NUM; i++)
    p_data->vars[i] = 0;
  p_data->wordsize = DEFAULT_WS;
  p_data->poison = 0;

  return p_data;
}

void print_program_data(program_data_t* p_data) {
  printf("\n***********************\n"); printf("Numbers Table\n");
  printf("nums:\n");
  hashtable_print(p_data->nums, stdout, item_print);
  printf("ten_bit_nums:\n");
  hashtable_print(p_data->ten_bit_nums, stdout, item_print);
  printf("\n***********************\n"); printf("Variables\n");
  for(int i = 0; i < VAR_NUM; i++) {
    printf("%c = ", 'a'+i);
    if (p_data->vars[i])
      printf("%s\n", p_data->vars[i]);
    else
      printf("0\n");
  }
  printf("wordsize: %d\n", p_data->wordsize);
}

void free_program_data(program_data_t* p_data) {
  hashtable_delete(p_data->nums, item_delete);
  for (int i = 0; i < VAR_NUM; i++)
    if (p_data->vars[i])
      free(p_data->vars[i]);
  free(p_data);
}

static hashtable_t* setup_ht() {
  hashtable_t* ht = hashtable_new(nums_SIZE);
  hashtable_insert(ht, "0", copy_number(_zero_, 1));
  return ht;
}

static void item_delete(void* item) {
  delete_number((number_t*)item);
}

static void item_print(FILE* fp, const char* key, void* item) {
  fprintf(fp, "key: %s\n", key);
  number_print((number_t*)item);
}

/*            nums_add_string           */ 
char* nums_add_string(program_data_t* prog_data, const char* number, type_e type) {
  
  int len;
  // get the binary key associated with the number
  char* key = malloc((prog_data->wordsize+1)*sizeof(char));
  const char* chopped = number;
  // chop off leading zeroes and hex signifier
  while(*chopped != '\0' && (*chopped == '0' || *chopped == 'x')) chopped++; 
  if (strlen(chopped) < 1)
    strcpy(key, "0");
  else {
    if (strlen(chopped) > prog_data->wordsize) {
#ifdef DEBUG
      printf("%s: the number is larger than wordsize", __FUNCTION__);
#endif
      return NULL;
    }
    unsigned long decimal;
    char raw_hex[100];
    switch (type) {
      case BINARY:
        len = strlen(chopped);
        for (int i = 0; i < prog_data->wordsize - len; i++)
          key[i] = '0';
        for (int i = prog_data->wordsize - len; i < prog_data->wordsize; i++)
          key[i] = chopped[i - prog_data->wordsize + len];
#ifdef DEBUG
#endif
        break;
      case DECIMAL:
        decimal = atol(chopped);
        if (ERROR == dec2binary(decimal, key, prog_data->wordsize)) {
#ifdef DEBUG
          printf("%s: decimal number larger than wordsize\n", __FUNCTION__);
#endif
          return NULL;
        }
        break;
      case HEXADECIMAL:
        strcpy(raw_hex, chopped);
        if (ERROR == hex2binary(raw_hex, key, prog_data->wordsize)) {
#ifdef DEBUG
          printf("%s: hex number larger than wordsize\n", __FUNCTION__);
#endif
          return NULL;
        }
#ifdef DEBUG
        printf("raw_hex: %s\n", raw_hex);
        printf("hex key: %s\n", key);
#endif  
        break;
      default:
        printf("error: invalid number type in add_string\n");
        return NULL;
    }
    // PAD the KEY with leading zeroes
    number_t* num = NULL;
#ifdef DEBUG
        printf("KEY: %s\n", key);
#endif     
    if((num = hashtable_find(prog_data->nums, key)) == NULL)
      hashtable_insert(prog_data->nums, key, new_number(type, chopped, prog_data->wordsize));
    else if (prog_data->wordsize != num->wordsize) {
      // change num wordsize
#ifdef DEBUG
      printf("changing wordsize from %d to %d\n", num->wordsize, prog_data->wordsize);
#endif
      change_wordsize(num, prog_data->wordsize);
    }
#ifdef DEBUG
    else printf("%s already in ht and wordsize is same\n", key);
#endif
  }
  
  return key; 
}

char* ten_bit_nums_add_string(program_data_t* prog_data, const char* decimal) {
  char *key = malloc((10+1)*sizeof(char));
  unsigned long dec = atol(decimal);
  if (dec > 2 << 10) {
    return NULL;
  }
  dec2binary(dec, key, 10);
  
  number_t *num = NULL;
  if((num = hashtable_find(prog_data->ten_bit_nums, key)) == NULL)
    hashtable_insert(prog_data->ten_bit_nums, key, new_number(DECIMAL, decimal, 10));
  return key;
}
 
/*            nums_add_number           */  
char* nums_add_number(program_data_t* prog_data, number_t* number) {
  char* key = malloc((number->wordsize+1)*sizeof(char));
  if (numbers_are_equal(number, _zero_)) {
    strcpy(key, "0");
    return key;
  }
  int i = 0;
  for(; i < number->wordsize; i++) {
    key[i] = number->bits[i];
  }
  key[i] = 0;
  if(!hashtable_insert(prog_data->nums, key, number)) {
#ifdef DEBUG
    printf("%s: key=%s already in the hashtable\n", __FUNCTION__, key);
#endif
  }
  return key;
}

/*            nums_get_num             */
number_t* nums_get_num(program_data_t* prog_data, const char* number) {
  number_t* num = hashtable_find(prog_data->nums, number);
  if(!num) {
    printf("this shouldnt happen\n");
  }
  return num;
}

number_t* ten_bit_nums_get_num(program_data_t* prog_data, char* key) {
  // pad key to 10 bits
  char *s;
  s = key;
  while(*s == '0') s++;
  char padded[11];
  padded[10] = '\0';
  int key_slen = strlen(s);
  for (int i = 0; i < 10 - key_slen; i++)
    padded[i] = '0';
  for (int i = 10 - key_slen; i <= 10; i++)
    padded[i] = s[i-(10-key_slen)];
  number_t *num = hashtable_find(prog_data->ten_bit_nums, padded);
  if(!num) {
    printf("%s: this shouldn't happen\n", __FUNCTION__);
    return NULL;
  }
  return num;
}


/*            vars_get_val            */ 
char* vars_get_val(program_data_t* prog_data, char var) {
  char* key;
  int idx = var - 'a';
  char* val = prog_data->vars[idx];
  if(val) {
    key = malloc((strlen(val)+1) * sizeof(char));
    strcpy(key, val);
  } else {
    key = malloc(2*sizeof(char));
    strcpy(key, "0");
  }
  return key;
}

/*            vars_set_val            */
void vars_set_val(program_data_t* prog_data, char var, char* val) {
  int idx = var - 'a';
  if(prog_data->vars[idx])
    free(prog_data->vars[idx]);
  prog_data->vars[idx] = malloc((strlen(val)+1)*sizeof(char));
  strcpy(prog_data->vars[idx], val);
}
