%{
#include "parser.tab.h"
#include <stdlib.h>
%}

%%

"("             { return LPAREN; }
")"             { return RPAREN; }
"not"           { return NOT; }
"and"           { return AND; }
"or"            { return OR; }
"=>"            { return IMP; }
"="             { return EQ; }
"forall"        { return FORALL; }
"exists"        { return EXISTS; }
"true"          { return TRUE; }
"false"         { return FALSE; }
\?[a-zA-Z][a-zA-Z0-9_]*  { yylval.str = strdup(yytext); return VAR; }
(">="|"<="|">"|"<")      { yylval.str = strdup(yytext); return VAR; }
[a-zA-Z][a-zA-Z0-9_]*    { yylval.str = strdup(yytext); return VAR; }
"-"?[0-9]+               { yylval.str = strdup(yytext); return NUMBER; }
("-"|"+"|"/"|"*")        { yylval.str = strdup(yytext); return VAR; }
[0-9]+                   { yylval.str = strdup(yytext); return NUMBER; }
[ \t\n]+                 { /* Whitespace ignorieren */ }
.                        { return *yytext; }
%%

int yywrap(void) {
    return 1;
}
