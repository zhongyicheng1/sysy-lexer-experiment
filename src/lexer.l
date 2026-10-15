%option noyywrap
%{
#include <stdio.h>
%}

%%

"int"       { printf("INT %s\n", yytext); }
"void"      { printf("VOID %s\n", yytext); }
"const"     { printf("CONST %s\n", yytext); }
"if"        { printf("IF %s\n", yytext); }
"else"      { printf("ELSE %s\n", yytext); }
"while"     { printf("WHILE %s\n", yytext); }
"return"    { printf("RETURN %s\n", yytext); }
"break"     { printf("BREAK %s\n", yytext); }
"continue"  { printf("CONTINUE %s\n", yytext); }

[a-zA-Z_][a-zA-Z0-9_]* {
    printf("ID %s\n", yytext);
}

[0-9]+ {
    printf("NUMBER %s\n", yytext);
}

"=="        { printf("EQ %s\n", yytext); }
"!="        { printf("NE %s\n", yytext); }
"<="        { printf("LE %s\n", yytext); }
">="        { printf("GE %s\n", yytext); }
"&&"        { printf("AND %s\n", yytext); }
"||"        { printf("OR %s\n", yytext); }

"+"         { printf("PLUS %s\n", yytext); }
"-"         { printf("MINUS %s\n", yytext); }
"*"         { printf("MULTI %s\n", yytext); }
"/"         { printf("DIV %s\n", yytext); }
"%"         { printf("MOD %s\n", yytext); }
"="         { printf("ASSIGN %s\n", yytext); }
"<"         { printf("LT %s\n", yytext); }
">"         { printf("GT %s\n", yytext); }
"!"         { printf("NOT %s\n", yytext); }

"("         { printf("LPAREN %s\n", yytext); }
")"         { printf("RPAREN %s\n", yytext); }
"["         { printf("LBRACKET %s\n", yytext); }
"]"         { printf("RBRACKET %s\n", yytext); }
"{"         { printf("LBRACE %s\n", yytext); }
"}"         { printf("RBRACE %s\n", yytext); }
","         { printf("COMMA %s\n", yytext); }
";"         { printf("SEMICOLON %s\n", yytext); }

"//".* {
}

"/*"([^*]|\*+[^*/])*\*+"/" {
}

[ \t\r\n]+ {
}

. {
    printf("ERROR %s\n", yytext);
}

%%

int main(void) {
    yylex();
    return 0;
}