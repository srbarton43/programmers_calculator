
%{
  #include "parser.tab.h"
  void yyerror(char* s, ...);
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
[10]+       { yylval.s_value = yytext; return BIN; }
[1-9][0-9]* { yylval.s_value = yytext; return DEC; }
0x[0-9a-f]+ { yylval.s_value = yytext; return HEX; }
\n          { return EOL; }
[ \t]       { /* ignore whitespace */ }
[a-z]       { yylval.c_value = yytext[0]; return VAR; }
.           { yyerror("unexpected character: %c", yytext[0]); yylval.c_value = yytext[0]; return (int)yytext[0]; }
%%
