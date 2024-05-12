%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <stdarg.h>

  #include "number.h"
  #include "utils.h"

  int yylex(void);
  int yylex_destroy(void);
  void yyerror(number_t* number, status_t *status, u64 *arg, const char *msg, ...);

  static const number_t _empty_num_ = {0};
%}

%code requires { 
  #include "number.h"
  #include "utils.h"
}

%parse-param {number_t *output} {status_t *status} {u64 *arg}

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

line: EOL
      {
        status->EMPTY = 1;
        YYACCEPT;
      }
    | statement EOL
      {
#ifdef DEBUG
        printf("line\n");
#endif
        *output = $1;
        YYACCEPT;
      }
    | error EOL {
        YYABORT;
      }
    ;

statement: QUIT EOL
            {
              status->QUIT_SIG = 1;
              YYACCEPT; // return from yyparse with 0 return code
            }
         | W_SIZE EOL
            {
              status->WSIZE_PR = 1;
              YYACCEPT;
            }
         | W_SIZE number EOL
            {
              *arg = (u64) $2.num;
              status->POISON = 0;
              status->WSIZE_CHG = 1;
              YYACCEPT;
            }
         | VAR '=' expression EOL
          {
#ifdef DEBUG
            printf("var assignment\n");
#endif
            status->VAR_ASSN = 1;
            *arg = (u64) $1;
            *output = $3;
            YYACCEPT;
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
                status->NUM_BUF_OF = 1;
              }
              prog_data->numbers_buf[prog_data->nbuf_ptr] = _empty_num_;
              add(&prog_data->numbers_buf[prog_data->nbuf_ptr], &$1, &$3, prog_data->wordsize);
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
                status->NUM_BUF_OF = 1;
              }
              prog_data->numbers_buf[prog_data->nbuf_ptr] = _empty_num_;
              sub(&prog_data->numbers_buf[prog_data->nbuf_ptr], &$3, &$1, prog_data->wordsize);
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
                status->NUM_BUF_OF = 1;
              }
              prog_data->numbers_buf[prog_data->nbuf_ptr] = _empty_num_;
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
                status->NUM_BUF_OF = 1;
              }
              prog_data->numbers_buf[prog_data->nbuf_ptr] = _empty_num_;
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
                status->NUM_BUF_OF = 1;
              }
              prog_data->numbers_buf[prog_data->nbuf_ptr] = _empty_num_;
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
                status->NUM_BUF_OF = 1;
              }
              prog_data->numbers_buf[prog_data->nbuf_ptr] = _empty_num_;
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
                status->NUM_BUF_OF = 1;
              }
              prog_data->numbers_buf[prog_data->nbuf_ptr] = _empty_num_;
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
                status->NUM_BUF_OF = 1;
              }
              prog_data->numbers_buf[prog_data->nbuf_ptr] = _empty_num_;
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
            status->NUM_BUF_OF = 1;
          }
          prog_data->numbers_buf[prog_data->nbuf_ptr] = _empty_num_;
          int ret = new_number(&prog_data->numbers_buf[prog_data->nbuf_ptr], DECIMAL, $1, prog_data->wordsize);
          if (ret == SUCCESS) {
            // pass
          } else {
            status->POISON = 1;
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
            status->NUM_BUF_OF = 1;
          }
          prog_data->numbers_buf[prog_data->nbuf_ptr] = _empty_num_;
          int ret = new_number(&prog_data->numbers_buf[prog_data->nbuf_ptr], HEXADECIMAL, $1, prog_data->wordsize);
          if (ret == SUCCESS) {
            // pass
          } else {
            status->POISON = 1;
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
            status->NUM_BUF_OF = 1;
          }
          prog_data->numbers_buf[prog_data->nbuf_ptr] = _empty_num_;
          int ret = new_number(&prog_data->numbers_buf[prog_data->nbuf_ptr], BINARY, $1, prog_data->wordsize);
          if (ret == SUCCESS) {
            // pass
          } else {
            status->POISON = 1;
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

void yyerror(number_t *number, status_t *status, u64 *arg, const char *msg, ...)
  {
  va_list args;

  //va_start (args, msg);
  //vfprintf (stderr, msg, args);
  //fprintf (stderr, "\n");
  //va_end (args);
}
