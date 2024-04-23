%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <stdarg.h>
  #include <stdbool.h>
  #include <string.h>
  
  #include "hashtable.h"
  #include "number.h"
  #include "utils.h"

  int yylex(void);
  int yylex_destroy(void);
  void yyerror(const char* s, ...);
%}

/* tokens */

%union {
  char* s_value;
  char c_value;
  int i_value;
}

%token <s_value> BIN DEC HEX
%token <i_value> QUIT W_SIZE EOL
%token <c_value> VAR
%type <c_value> '-' '+' '~' '=' '&' '|'

%type <s_value> number expression statement

%left '|'
%left '&'
%left <i_value> LSHIFT RSHIFT
%left '+' '-'
%precedence NEG '~'   /* negation--unary minus and bitwise NOT */

/*  grammar  */
%%

input: /*nothing*/
     | input line { printf(">>> "); }
     ;

line: EOL
    | statement EOL 
      {
#ifdef DEBUG
        printf("line\n");
#endif
        
        if ($1 == NULL || prog_data->poison) {
          printf("Error...\n");
        } else if (strcmp("foo", $1) == 0) {
          // wsize change
        } else if (strcmp("bar", $1) == 0) {
          // var assignment
        } else {
          number_t* num = nums_get_num(prog_data, $1);
          printf("  =\n");
          number_print(num);
          free($1);
        }
        prog_data->poison = 0;
      }
    | error EOL
    ;

statement: QUIT
            {
              YYACCEPT; // return from yyparse with 0 return code
            }
         | W_SIZE
            {
              printf("The current wordsize is %d\n", prog_data->wordsize);
              $$ = "foo";
            }
         | W_SIZE number
            {
              long long new_wsize = number_getSdec(ten_bit_nums_get_num(prog_data, $2));
              if (new_wsize < 4 || new_wsize > 64) {
                printf("unsupported wordsize: %lld\n", new_wsize);
              } else {
                printf("changed wordsize to %lld\n", new_wsize);
                prog_data->wordsize = new_wsize;
              }
              prog_data->poison = 0;
              $$ = "foo";
            }
         | VAR '=' expression
          {
#ifdef DEBUG
            printf("var assignment\n");
#endif
            vars_set_val(prog_data, $1, $3);
            number_t* num = nums_get_num(prog_data, $3);
            free($3);
            printf("%c\n  = \n", $1);
            number_print(num);
            $$ = "bar";
          }
         | expression 
            {
#ifdef DEBUG
              printf("expression\n");
#endif
              $$ = $1;
            }
         ;

expression: number
          
          | expression '+' expression
            { 
#ifdef DEBUG
              printf("adding\n");
#endif
              number_t* num = add(nums_get_num(prog_data, $1), nums_get_num(prog_data, $3), prog_data->wordsize);
              free($1); free($3);
              char* key = nums_add_number(prog_data, num);
              if (global_nums_flag.overflow)
                printf("There was an overflow\n");
              $$ = key;                                     
            }
          | expression '-' expression 
            { 
#ifdef DEBUG
              printf("subtracting\n");
#endif
              number_t* num = sub(nums_get_num(prog_data, $3), nums_get_num(prog_data, $1), prog_data->wordsize);
              free($3); free($1);
              char* key = nums_add_number(prog_data, num);
              if (global_nums_flag.overflow)
                printf("There was an underflow\n");
              $$ = key;
            }
          | expression RSHIFT number  
            { 
#ifdef DEBUG
              printf("rshift\n"); 
#endif
              number_t* num = rshift(nums_get_num(prog_data, $1), nums_get_num(prog_data, $3), prog_data->wordsize);
              free($1); free($3);
              char* key = nums_add_number(prog_data, num);
              $$ = key;
            }
          | expression LSHIFT number
            {
#ifdef DEBUG
              printf("lshift\n");
#endif
              number_t* num = lshift(nums_get_num(prog_data, $1), nums_get_num(prog_data, $3), prog_data->wordsize);
              free($1); free($3);
              //printf("result: \n"); number_print(num);
              char* key = nums_add_number(prog_data, num);
              $$ = key;
            }
          | expression '&' expression
            {
#ifdef DEBUG
              printf("and\n");
#endif
              number_t* num = and(nums_get_num(prog_data, $1), nums_get_num(prog_data, $3), prog_data->wordsize);
              free($1); free($3);
              char* key = nums_add_number(prog_data, num);
              $$ = key;
            }
          | expression '|' expression
            {
#ifdef DEBUG
              printf("or\n");
#endif
              number_t* num = or(nums_get_num(prog_data, $1), nums_get_num(prog_data, $3), prog_data->wordsize);
              free($1); free($3);
              char* key = nums_add_number(prog_data, num);
              $$ = key;
            }
          | '-' expression %prec NEG
            {
#ifdef DEBUG
              printf("negation\n");
#endif
              number_t* num = twos_comp(nums_get_num(prog_data, $2), prog_data->wordsize);
              free($2);
              char* key = nums_add_number(prog_data, num);
              $$ = key;
            }
          | '~' expression
            {
#ifdef DEBUG
              printf("bitwise NOT\n");
#endif
              number_t* num = ones_comp(nums_get_num(prog_data, $2), 0);
              free($2);
              char* key = nums_add_number(prog_data, num);
              $$ = key;
            }
          | '(' expression ')'
            {
              $$ = $2;
            }
          ;

number: DEC 
        {
#ifdef DEBUG
          printf("decimal\n");
#endif
          char *ten_bit_key = ten_bit_nums_add_string(prog_data, $1);
          char* key = nums_add_string(prog_data, $1, DECIMAL);
          if (key) {
            $$ = key;
          } else {
            $$ = ten_bit_key;
            prog_data->poison = 1;
          }
        }
      | HEX
        {
#ifdef DEBUG
          printf("hex\n");
#endif
          char* key = nums_add_string(prog_data, $1, HEXADECIMAL);
          $$ = key;
        }
      | BIN
        {
#ifdef DEBUG
          printf("binary\n");
#endif
          char* key = nums_add_string(prog_data, $1, BINARY);
          $$ = key;
        }
      | VAR 
        {
#ifdef DEBUG
          printf("variable %c\n", $1);
#endif
          char* key = vars_get_val(prog_data, $1);
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
