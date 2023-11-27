%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <stdarg.h>
  
  #include "hashtable.h"
  #include "number.h"
  #include "utils.h"

  hashtable_t* ht;
  
  int yylex(void);
  int yylex_destroy(void);
  void yyerror(const char* s, ...);
  
  char* add_number(const char* number, type_e type);

  #define WORDSIZE 8
  
%}

/* tokens */

%union {
  char* s_value;
  char c_value;
  int i_value;
}

%token <s_value> BIN DEC HEX VAR
%token <i_value> LSHIFT RSHIFT
%token <i_value> QUIT ECHO_N EOL
%token <c_value> ADD SUB

%type <c_value> '+' '-'
%type <s_value> number expression statement

%left '-' '+' RSHIFT LSHIFT

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
         | ECHO_N number { printf("echoing %s\n", $2); }
         | expression { printf("expression\n"); }
         ;

expression: number
          | expression RSHIFT number  { printf("rshift\n"); }
          | expression LSHIFT number  { printf("lshift\n"); }
          | expression '+' expression { printf("adding\n"); }
          | expression '-' expression { printf("subtracting\n"); }
          ;

number: DEC { printf("decimal\n"); $$ = $1; }
      | HEX { printf("hex\n"); $$ = $1; }
      | BIN { printf("binary\n"); add_number($1, BINARY); $$ = $1; }
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


char* add_number(const char* number, type_e type) {
  char* key;
  switch (type) {
    case BINARY:
      key = number;
      break;
    default:
      printf("error\n");
    
  }
  hashtable_insert(ht, key, new_number(type, number, WORDSIZE));
  return key; 
}

