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
  char *s_value;
  number_t n_value;
  char c_value;
  int i_value;
}

%token <s_value> BIN DEC HEX
%token <i_value> QUIT W_SIZE EOL
%token <c_value> VAR
%type <c_value> '-' '+' '~' '=' '&' '|'

%type <n_value> number expression statement

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

        if (prog_data->poison) {
          printf("Error...\n");
        } else if (1 == prog_data->buf_overflow_check) {
          printf("Error: More numbers than the program can handle\n");
        } else if (6 == prog_data->status) {
          // wsize change
        } else if (5 == prog_data->status) {
          // var assignment
        } else if (-1 == prog_data->status) {
          printf("Error: TODO make this message better\n");
        } else {
          printf("  =\n");
          number_print(&$1);
        }
        prog_data->poison = 0;
        prog_data->status = 0;
        prog_data->nbuf_ptr = 0;
        prog_data->buf_overflow_check = 0;
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
              prog_data->status = 6;
            }
         | W_SIZE number
            {
              long long new_wsize = $2.num;
              if (new_wsize < 4 || new_wsize > 64) {
                printf("unsupported wordsize: %lld\n", new_wsize);
              } else {
                printf("changed wordsize to %lld\n", new_wsize);
                prog_data->wordsize = new_wsize;
              }
              prog_data->poison = 0;
              prog_data->status = 6;
            }
         | VAR '=' expression
          {
#ifdef DEBUG
            printf("var assignment\n");
#endif
            //vars_set_val(prog_data, $1, $3);
            vars_set_num(prog_data, $1, &$3);
            printf("%c\n  = \n", $1);
            number_print(&$3);
            prog_data->status = 5;
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
              //number_t* num = add(nums_get_num(prog_data, $1), nums_get_num(prog_data, $3), prog_data->wordsize);
              if (MAX_NUMBERS_COUNT == prog_data->nbuf_ptr) {
                prog_data->nbuf_ptr--;
                prog_data->buf_overflow_check = 1;
              }
              add(&prog_data->numbers_buf[prog_data->nbuf_ptr], &$1, &$3, prog_data->wordsize);
              if (global_nums_flag.overflow)
                printf("There was an overflow\n");
              $$ = prog_data->numbers_buf[prog_data->nbuf_ptr++];
            }
          | expression '-' expression
            {
#ifdef DEBUG
              printf("subtracting\n");
#endif
              //number_t* num = sub(nums_get_num(prog_data, $3), nums_get_num(prog_data, $1), prog_data->wordsize);
              //char* key = nums_add_number(prog_data, num);
              if (MAX_NUMBERS_COUNT == prog_data->nbuf_ptr) {
                prog_data->nbuf_ptr--;
                prog_data->buf_overflow_check = 1;
              }
              sub(&prog_data->numbers_buf[prog_data->nbuf_ptr], &$3, &$1, prog_data->wordsize);
              if (global_nums_flag.overflow)
                printf("There was an underflow\n");
              $$ = prog_data->numbers_buf[prog_data->nbuf_ptr++];
            }
          | expression RSHIFT number
            {
#ifdef DEBUG
              printf("rshift\n");
#endif
              //number_t* num = rshift(nums_get_num(prog_data, $1), nums_get_num(prog_data, $3), prog_data->wordsize);
              //free($1); free($3);
              //char* key = nums_add_number(prog_data, num);
              //$$ = key;
              if (MAX_NUMBERS_COUNT == prog_data->nbuf_ptr) {
                prog_data->nbuf_ptr--;
                prog_data->buf_overflow_check = 1;
              }
              rshift(&prog_data->numbers_buf[prog_data->nbuf_ptr], &$1, &$3, prog_data->wordsize);
              $$ = prog_data->numbers_buf[prog_data->nbuf_ptr++];
            }
          | expression LSHIFT number
            {
#ifdef DEBUG
              printf("lshift\n");
#endif
              //number_t* num = lshift(nums_get_num(prog_data, $1), nums_get_num(prog_data, $3), prog_data->wordsize);
              //free($1); free($3);
              if (MAX_NUMBERS_COUNT == prog_data->nbuf_ptr) {
                prog_data->nbuf_ptr--;
                prog_data->buf_overflow_check = 1;
              }
              lshift(&prog_data->numbers_buf[prog_data->nbuf_ptr], &$1, &$3, prog_data->wordsize);
              //printf("result: \n"); number_print(num);
              //char* key = nums_add_number(prog_data, num);
              $$ = prog_data->numbers_buf[prog_data->nbuf_ptr++];
            }
          | expression '&' expression
            {
#ifdef DEBUG
              printf("and\n");
#endif
              //number_t* num = and(nums_get_num(prog_data, $1), nums_get_num(prog_data, $3), prog_data->wordsize);
              //free($1); free($3);
              //char* key = nums_add_number(prog_data, num);
              if (MAX_NUMBERS_COUNT == prog_data->nbuf_ptr) {
                prog_data->nbuf_ptr--;
                prog_data->buf_overflow_check = 1;
              }
              and(&prog_data->numbers_buf[prog_data->nbuf_ptr], &$1, &$3, prog_data->wordsize);
              $$ = prog_data->numbers_buf[prog_data->nbuf_ptr++];
            }
          | expression '|' expression
            {
#ifdef DEBUG
              printf("or\n");
#endif
              //number_t* num = or(nums_get_num(prog_data, $1), nums_get_num(prog_data, $3), prog_data->wordsize);
              //free($1); free($3);
              //char* key = nums_add_number(prog_data, num);
              //$$ = key;
              if (MAX_NUMBERS_COUNT == prog_data->nbuf_ptr) {
                prog_data->nbuf_ptr--;
                prog_data->buf_overflow_check = 1;
              }
              or(&prog_data->numbers_buf[prog_data->nbuf_ptr], &$1, &$3, prog_data->wordsize);
              $$ = prog_data->numbers_buf[prog_data->nbuf_ptr++];
            }
          | '-' expression %prec NEG
            {
#ifdef DEBUG
              printf("negation\n");
#endif
              //number_t* num = twos_comp(nums_get_num(prog_data, $2), prog_data->wordsize);
              //free($2);
              //char* key = nums_add_number(prog_data, num);
              //$$ = key;
              if (MAX_NUMBERS_COUNT == prog_data->nbuf_ptr) {
                prog_data->nbuf_ptr--;
                prog_data->buf_overflow_check = 1;
              }
              twos_comp(&prog_data->numbers_buf[prog_data->nbuf_ptr], &$2, prog_data->wordsize);
              $$ = prog_data->numbers_buf[prog_data->nbuf_ptr++];
            }
          | '~' expression
            {
#ifdef DEBUG
              printf("bitwise NOT\n");
#endif
              //number_t* num = ones_comp(nums_get_num(prog_data, $2), prog_data->wordsize);
              //free($2);
              //char* key = nums_add_number(prog_data, num);
              //$$ = key;
              if (MAX_NUMBERS_COUNT == prog_data->nbuf_ptr) {
                prog_data->nbuf_ptr--;
                prog_data->buf_overflow_check = 1;
              }
              ones_comp(&prog_data->numbers_buf[prog_data->nbuf_ptr], &$2, prog_data->wordsize);
              $$ = prog_data->numbers_buf[prog_data->nbuf_ptr++];
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
          //char *ten_bit_key = ten_bit_nums_add_string(prog_data, $1);
          //char* key = nums_add_string(prog_data, $1, DECIMAL);
          if (MAX_NUMBERS_COUNT == prog_data->nbuf_ptr) {
            prog_data->nbuf_ptr--;
            prog_data->buf_overflow_check = 1;
          }
          int ret = new_number(&prog_data->numbers_buf[prog_data->nbuf_ptr], DECIMAL, $1, prog_data->wordsize);
          if (ret == SUCCESS) {
            // pass
          } else {
            prog_data->poison = 1;
          }
          $$ = prog_data->numbers_buf[prog_data->nbuf_ptr++];
        }
      | HEX
        {
#ifdef DEBUG
          printf("hex\n");
#endif
          //char* key = nums_add_string(prog_data, $1, HEXADECIMAL);
          if (MAX_NUMBERS_COUNT == prog_data->nbuf_ptr) {
            prog_data->nbuf_ptr--;
            prog_data->buf_overflow_check = 1;
          }
          int ret = new_number(&prog_data->numbers_buf[prog_data->nbuf_ptr], HEXADECIMAL, $1, prog_data->wordsize);
          if (ret == SUCCESS) {
            // pass
          } else {
            prog_data->poison = 1;
          }
          $$ = prog_data->numbers_buf[prog_data->nbuf_ptr++];
        }
      | BIN
        {
#ifdef DEBUG
          printf("binary\n");
#endif
          //char* key = nums_add_string(prog_data, $1, BINARY);
          if (MAX_NUMBERS_COUNT == prog_data->nbuf_ptr) {
            prog_data->nbuf_ptr--;
            prog_data->buf_overflow_check = 1;
          }
          int ret = new_number(&prog_data->numbers_buf[prog_data->nbuf_ptr], BINARY, $1, prog_data->wordsize);
          if (ret == SUCCESS) {
            // pass
          } else {
            prog_data->poison = 1;
          }
          $$ = prog_data->numbers_buf[prog_data->nbuf_ptr++];
        }
      | VAR
        {
#ifdef DEBUG
          printf("variable %c\n", $1);
#endif
          $$ = vars_get_num(prog_data, $1);
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
