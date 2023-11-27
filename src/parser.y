%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <stdarg.h>
  #include <stdbool.h>
  #include <string.h>
  
  #include "hashtable.h"
  #include "number.h"
  #include "utils.h"

  hashtable_t* ht;
  
  int yylex(void);
  int yylex_destroy(void);
  void yyerror(const char* s, ...);
  
  char* ht_add_string(const char* number, type_e type);
  char* ht_add_number(number_t* number);
  number_t* ht_get_num(const char* number);

  #define WORDSIZE 8
  
%}

/* tokens */

%union {
  char* s_value;
  char c_value;
  int i_value;
}

%token <s_value> BIN DEC HEX VAR
%token <i_value> QUIT ECHO_N EOL
%token <c_value> ADD SUB

%type <s_value> number expression statement

%left <c_value> '-' '+'
%left <i_value> LSHIFT RSHIFT

/*  grammar  */
%%

input: /*nothing*/
     | input line
     ;

line: EOL
    | statement EOL { printf("line\n"); }
    | error EOL
    ;

statement: QUIT { exit(0); }
         | ECHO_N number 
            { 
              printf("echoing %s\n", $2);
              number_t* num = ht_get_num($2);
              number_print(num);
            }
         | expression 
            {
              printf("expression\n");
              number_t* num = ht_get_num($1);
              number_print(num);
            }
         ;

expression: number
          | expression '+' expression
            { 
              printf("adding\n");
              number_t* num = add(ht_get_num($1), ht_get_num($3));
              //printf("result: \n"); number_print(num);
              char* key = ht_add_number(num);
              $$ = key;                                     
            }
          | expression '-' expression 
            { 
              printf("subtracting\n");
              number_t* num = sub(ht_get_num($3), ht_get_num($1));
              //printf("result: \n"); number_print(num);
              char* key = ht_add_number(num);
              $$ = key;
            }
          | expression RSHIFT number  
            { 
              printf("rshift\n"); 
              number_t* num = rshift(ht_get_num($1), ht_get_num($3));
              //printf("result: \n"); number_print(num);
              char* key = ht_add_number(num);
              $$ = key;
            }
          | expression LSHIFT number
            {
              printf("lshift\n");
              number_t* num = lshift(ht_get_num($1), ht_get_num($3));
              //printf("result: \n"); number_print(num);
              char* key = ht_add_number(num);
              $$ = key;
            }
          ;

number: DEC { printf("decimal\n"); char* key = ht_add_string($1, DECIMAL); $$ = key; }
      | HEX { printf("hex\n"); $$ = $1; }
      | BIN { printf("binary\n"); char* key = ht_add_string($1, BINARY); $$ = key; }
      ;

%%

// int main(int argc, char** argv) {
//   int ret = yyparse();
//   yylex_destroy();
//   return ret;
// }

void yyerror(const char* str, ...)
  {
  va_list args;

  va_start (args, str);
//fprintf (stderr,"%d: ", line_no);
  vfprintf (stderr, str, args);
  fprintf (stderr, "\n");
  va_end (args);
}


/*
 * ht_add_string - adds bitstring->number pair to ht
 * 
 * params:
 *    const char* number  := string representing number
 *    type_e type         := enum type of number 
 * returns:
 *    char* key           := bitstring used as key in ht
 * 
 * caller must:
 *    free return val at some point
 */
char* ht_add_string(const char* number, type_e type) {
  
  // get the binary key associated with the number
  char* key = malloc(100*sizeof(char)); // TODO free this memory later
  unsigned long decimal;
  switch (type) {
    case BINARY:
      strcpy(key, number);
      break;
    case DECIMAL:
      decimal = atol(number);
      dec2binary(decimal, key);
      break;
    default: // TODO hex and dec
      printf("error\n");
      yyerror("key is bad");
      return NULL;
  }
  
  bool ret = hashtable_insert(ht, key, new_number(type, number, WORDSIZE));
  if (!ret) printf("%s already in ht\n", key);
  return key; 
}
 
/*
 * ht_add_number - adds bitstring->number pair to ht
 * 
 * params:
 *    number_t* number  := string representing number
 * returns:
 *    char* key         := bitstring used as key in ht
 * 
 * caller must:
 *    free return val at some point
 */ 
char* ht_add_number(number_t* number) {
  char* key = malloc(100*sizeof(char)); // TODO free this later
  int i = 0;
  for(; i < number->len; i++) {
    key[i] = number->bits[i + number->wordsize - number->len];
  }
  key[i] = 0;
  hashtable_insert(ht, key, number);
  return key;
}

/*
 * ht_get_num - gets number from hashtable
 * 
 * params:
 *    const char* number  := bitstring as key
 * returns:
 *    number_t* num       := pointer to the number
 */
number_t* ht_get_num(const char* number) {
  number_t* num = hashtable_find(ht, number);
  if(!num) {
    printf("this shouldnt happen\n");
  }
  return num;
}
