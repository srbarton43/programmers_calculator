
%{
  #include "parser.tab.h"
  void yyerror(char* s, ...);
%}

%option nounput
%option noinput

%%
quit        { return(QUIT); }
echo        { return(ECHO_N); }
">>"        { return(RSHIFT); }
"<<"        { return(LSHIFT); }
"+"|"-"     { yylval.c_value = yytext[0]; return (int)yytext[0]; }
[10]+      { yylval.s_value = yytext; return BIN; }
[1-9][0-9]* { yylval.s_value = yytext; return DEC; }
0x[0-9a-f]+ { yylval.s_value = yytext; return HEX; }
\n          { return EOL; }
[ \t]       { /* ignore whitespace */ }
[a-zA-Z]    { yylval.s_value = yytext; return VAR; }
.           { yyerror("unexpected character"); }
%%
//[0-9]+      { yylval.s_value = yytext; return DEC; }
/* .           { yyerror("unexpected character"); } */
// .           {
//               yylval.c_value = yytext[0];
//               if (yytext[0] < ' ')
//                 yyerror ("illegal character: ^%c",yytext[0] + '@');
//               else
//                 if (yytext[0] > '~')
//                   yyerror ("illegal character: \\%03o", (int) yytext[0]);
//                 else
//                   yyerror ("illegal character: %s",yytext);
//             }
// 
