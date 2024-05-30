%{
  #include "number.h"
  #include "utils.h"
  #include "parser.tab.h"
  void yyerror(number_t *number, status_t *status, u64 *arg, char* msg, ...);
%}

%option nounput
%option noinput

%%
quit        { return(QUIT); }
ws          { return(W_SIZE); }
">>"        { return(RSHIFT); }
"<<"        { return(LSHIFT); }
"+"|"-"|"~"|"("|")"|"="|"&"|"|" { yylval.c_value = yytext[0]; 
              return (int)yytext[0]; }
0b[10]+       { yylval.s_value = &yytext[2]; return BIN; }
[0-9]+ { yylval.s_value = yytext; return DEC; }
0x[0-9a-f]+ { yylval.s_value = &yytext[2]; return HEX; }
\n          { return EOL; }
[ \t]       { /* ignore whitespace */ }
[a-z]       { yylval.c_value = yytext[0]; return VAR; }
.           { yyerror(NULL, NULL, NULL, "unexpected character: %c", yytext[0]); yylval.c_value = yytext[0]; return (int)yytext[0]; }
%%
