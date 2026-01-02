%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <stdarg.h>

  #include "number.h"
  #include "utils.h"

  int yylex(void);
  int yylex_destroy(void);
  void yyerror(number_flag_t *number, status_t *status, u64 *arg, const char *msg, ...);
%}

%code requires { 
  #include "number.h"
  #include "utils.h"
}

%parse-param {number_flag_t *output} {status_t *status} {u64 *arg}

/* tokens */

%union {
  char *s_value;
  number_flag_t n_value;
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
        printf("num=%p\n", $1.number);
        number_debug($1.number);
#endif
        *output = $1;
        YYACCEPT;
      }
    | expression YYEOF
      {
#ifdef DEBUG
        printf("evaluate expr\n");
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
              *arg = (u64) n_getLeastSigChunk($2.number);
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
              printf("num=%p\n", $1.number);
              number_debug($1.number);
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
              number_t *new_num = NULL;
              number_alloc(&new_num);
              add(new_num, $1.number, $3.number, g_prog_data.wordsize);
              if ($1.flag == NOT_VAR)
                number_destroy($1.number);
              if ($3.flag == NOT_VAR)
                number_destroy($3.number);
              number_flag_t out = {new_num, NOT_VAR};
              $$ = out; 
            }
          | expression '-' expression
            {
#ifdef DEBUG
              printf("subtracting\n");
#endif
              number_t *new_num = NULL;
              number_alloc(&new_num);
              sub(new_num, $3.number, $1.number, g_prog_data.wordsize);
              if ($1.flag == NOT_VAR)
                number_destroy($1.number);
              if ($3.flag == NOT_VAR)
                number_destroy($3.number);
              number_flag_t out = {new_num, NOT_VAR};
              $$ = out; 
            }
          | expression RSHIFT number
            {
#ifdef DEBUG
              printf("rshift\n");
#endif
              number_t *new_num = NULL;
              number_alloc(&new_num);
              rshift(new_num, $1.number, $3.number, g_prog_data.wordsize);
              if ($1.flag == NOT_VAR)
                number_destroy($1.number);
              if ($3.flag == NOT_VAR)
                number_destroy($3.number);
              number_flag_t out = {new_num, NOT_VAR};
              $$ = out; 
            }
          | expression LSHIFT number
            {
#ifdef DEBUG
              printf("lshift\n");
#endif
              number_t *new_num = NULL;
              number_alloc(&new_num);
              lshift(new_num, $1.number, $3.number, g_prog_data.wordsize);
              if ($1.flag == NOT_VAR)
                number_destroy($1.number);
              if ($3.flag == NOT_VAR)
                number_destroy($3.number);
              number_flag_t out = {new_num, NOT_VAR};
              $$ = out; 
            }
          | expression '&' expression
            {
#ifdef DEBUG
              printf("and\n");
#endif
              number_t *new_num = NULL;
              number_alloc(&new_num);
              and(new_num, $1.number, $3.number, g_prog_data.wordsize);
              if ($1.flag == NOT_VAR)
                number_destroy($1.number);
              if ($3.flag == NOT_VAR)
                number_destroy($3.number);
              number_flag_t out = {new_num, NOT_VAR};
              $$ = out; 
            }
          | expression '|' expression
            {
#ifdef DEBUG
              printf("or\n");
#endif
              number_t *new_num = NULL;
              number_alloc(&new_num);
              or(new_num, $1.number, $3.number, g_prog_data.wordsize);
              if ($1.flag == NOT_VAR)
                number_destroy($1.number);
              if ($3.flag == NOT_VAR)
                number_destroy($3.number);
              number_flag_t out = {new_num, NOT_VAR};
              $$ = out; 
            }
          | '-' expression %prec NEG
            {
#ifdef DEBUG
              printf("negation\n");
#endif
              number_t *new_num = NULL;
              number_alloc(&new_num);
              twos_comp(new_num, $2.number, g_prog_data.wordsize);
              if ($2.flag == NOT_VAR)
                number_destroy($2.number);
              number_flag_t out = {new_num, NOT_VAR};
              $$ = out; 
            }
          | '~' expression
            {
#ifdef DEBUG
              printf("bitwise NOT\n");
#endif
              number_t *new_num = NULL;
              number_alloc(&new_num);
              ones_comp(new_num, $2.number, g_prog_data.wordsize);
              if ($2.flag == NOT_VAR)
                number_destroy($2.number);
              number_flag_t out = {new_num, NOT_VAR};
              $$ = out; 
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
          
          number_t *new_num = NULL;
          number_alloc(&new_num);
#ifdef DEBUG
          printf("before: new_num=%p\n", new_num);
          number_debug(new_num);
#endif
          int ret = new_number(new_num, DECIMAL, $1, g_prog_data.wordsize);
#ifdef DEBUG
          printf("after: new_num=%p\n", new_num);
          number_debug(new_num);
#endif
          if (ret == SUCCESS) {
            // pass
          } else {
            status->POISON = 1;
          }
          number_flag_t out = {new_num, NOT_VAR};
          $$ = out;
        }
      | HEX
        {
#ifdef DEBUG
          printf("hex\n");
#endif
          number_t *new_num = NULL;
          number_alloc(&new_num);
          int ret = new_number(new_num, HEXADECIMAL, $1, g_prog_data.wordsize);
          if (ret == SUCCESS) {
            // pass
          } else {
            status->POISON = 1;
          }
          number_flag_t out = {new_num, NOT_VAR};
          $$ = out;
        }
      | BIN
        {
#ifdef DEBUG
          printf("binary\n");
#endif
          number_t *new_num = NULL;
          number_alloc(&new_num);
          int ret = new_number(new_num, BINARY, $1, g_prog_data.wordsize);
          if (ret == SUCCESS) {
            // pass
          } else {
            status->POISON = 1;
          }
          number_flag_t out = {new_num, NOT_VAR};
          $$ = out;
        }
      | VAR 
        {
#ifdef DEBUG
          printf("variable %c\n", $1);
#endif
          number_t *var_value = NULL;
          if (SUCCESS == vars_get_num(&var_value, $1)) {
#ifdef DEBUG
            printf("number=%p\n", var_value);
#endif
            status->ACCESS_VAR = 1;
            number_flag_t out = {var_value, IS_VAR};
            $$ = out;
          } else {
            status->UNDEF_VAR = 1;
            YYACCEPT;
          }
        }
      ;

%%

void yyerror(number_flag_t *number, status_t *status, u64 *arg, const char *msg, ...) {
}
