%{
  #include <stdio.h>
  #include <stdlib.h>
  int yylex(void);
  int yylex_destroy(void);
  void yyerror(const char* s);
%}

/* tokens */

%union {
  char* s_value;
  char c_value;
  int i_value;
}

%token <s_value> BIN DEC HEX
%token <i_value> LSHIFT RSHIFT
%token <i_value> QUIT ECHO_N EOL
%token <c_value> ADD SUB

%type <c_value> '+' '-'
%type <s_value> number expression

/*  grammar  */
%%

input: /*nothing*/
     | input line EOL
     | input error EOL
     ;

line: statement
    | expression
    ;

statement: QUIT { exit(0); }
         | ECHO_N number { printf("echoing %s\n", $2); }
         ;

expression: number
          | number '+' number { printf("adding\n"); }
          | number '-' number { printf("subtracting\n"); }
          ;

number: DEC { printf("decimal\n"); $$ = $1; }
      | HEX { printf("hex\n"); $$ = $1; }
      | BIN { printf("binary\n"); $$ = $1; }
      ;

%%

int main(int argc, char** argv) {
  int ret = yyparse();
  yylex_destroy();
  return ret;
}

void yyerror(const char* s) {
  fprintf(stderr, "error: %s\n", s);
}
