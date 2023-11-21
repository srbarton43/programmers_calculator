
%{
  #include "parser.tab.h"
  void yyerror(char* s);
%}

%option nounput
%option noinput

%%
quit        { return QUIT; }
echo        { return ECHO_N; }
">>"        { return(RSHIFT); }
"<<"        { return(LSHIFT); }
"+"|"-"     { yylval.c_value = yytext[0]; return (int)yytext[0]; }
1[10]+      { yylval.s_value = yytext; return BIN; }
[0-9]+      { yylval.s_value = yytext; return DEC; }
0x[0-9a-f]+ { yylval.s_value = yytext; return HEX; }
\n          { return EOL; }
[ \t]       { /* ignore whitespace */ }
.           { yyerror("unexpected character"); }
%%
/* .           { yylval.c_value = yytext[0]; printf("Other character %s\n", yytext); } */
