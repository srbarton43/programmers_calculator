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
  
%}

/* tokens */

%union {
  char* s_value;
  char c_value;
  int i_value;
}

%token <s_value> BIN DEC HEX VAR
%token <i_value> QUIT ECHO_N EOL
%type <c_value> '-' '+'

%type <s_value> number expression statement

%left '+' '-'
%left <i_value> LSHIFT RSHIFT
%precedence NEG   /* negation--unary minus */

/*  grammar  */
%%

input: /*nothing*/
     | input line
     ;

line: EOL
    | statement EOL 
      {
#ifdef DEBUG
        printf("line\n");
#endif
      }
    | error EOL
    ;

statement: QUIT
            {
              YYACCEPT; // return from yyparse with 0 return code
            }
         | ECHO_N number 
            { 
#ifdef DEBUG
              printf("echoing %s\n", $2);
#endif
              number_t* num = ht_get_num($2);
              number_print(num);
            }
         | expression 
            {
#ifdef DEBUG
              printf("expression\n");
#endif
              number_t* num = ht_get_num($1);
              printf("  =\n");
              number_print(num);
            }
         ;

expression: number
          | expression '+' expression
            { 
#ifdef DEBUG
              printf("adding\n");
#endif
              number_t* num = add(ht_get_num($1), ht_get_num($3));
              //printf("result: \n"); number_print(num);
              char* key = ht_add_number(num);
              $$ = key;                                     
            }
          | expression '-' expression 
            { 
#ifdef DEBUG
              printf("subtracting\n");
#endif
              number_t* num = sub(ht_get_num($3), ht_get_num($1));
              char* key = ht_add_number(num);
              $$ = key;
            }
          | expression RSHIFT number  
            { 
#ifdef DEBUG
              printf("rshift\n"); 
#endif
              number_t* num = rshift(ht_get_num($1), ht_get_num($3));
              char* key = ht_add_number(num);
              $$ = key;
            }
          | expression LSHIFT number
            {
#ifdef DEBUG
              printf("lshift\n");
#endif
              number_t* num = lshift(ht_get_num($1), ht_get_num($3));
              //printf("result: \n"); number_print(num);
              char* key = ht_add_number(num);
              $$ = key;
            }
          | '-' expression %prec NEG
            {
#ifdef DEBUG
              printf("negation\n");
#endif
              number_t* num = twos_comp(ht_get_num($2), 0);
              char* key = ht_add_number(num);
              $$ = key;
            }
          ;

number: DEC 
        {
#ifdef DEBUG
          printf("decimal\n");
#endif
          char* key = ht_add_string($1, DECIMAL);
          $$ = key;
        }
      | HEX
        {
#ifdef DEBUG
          printf("hex\n");
#endif
          char* key = ht_add_string($1, HEXADECIMAL);
          $$ = key;
        }
      | BIN
        {
#ifdef DEBUG
          printf("binary\n");
#endif
          char* key = ht_add_string($1, BINARY);
          $$ = key;
        }
      ;

%%

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
  const char* chopped = number;
  while(chopped != 0 && (*chopped == '0' || *chopped == 'x')) chopped++;
  if (strlen(chopped) < 1)
    strcpy(key, "0");
  else {
    unsigned long decimal;
    char raw_hex[100];
    switch (type) {
      case BINARY:
        strcpy(key, chopped);
        break;
      case DECIMAL:
        decimal = atol(chopped);
        dec2binary(decimal, key);
        break;
      case HEXADECIMAL:
        strcpy(raw_hex, chopped);
        printf("raw_hex: %s\n", raw_hex);
        hex2binary(raw_hex, key);
        printf("hex key: %s\n", key);
        break;
      default:
        printf("error\n");
        yyerror("key is bad");
        return NULL;
    }
#ifdef DEBUG
    bool ret = 
#endif     
    hashtable_insert(ht, key, new_number(type, chopped, WORDSIZE));
#ifdef DEBUG
    if (!ret) printf("%s already in ht\n", key);
#endif
  }
  
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
  if (numbers_are_equal(number, _zero_)) {
    strcpy(key, "0");
    return key;
  }
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
