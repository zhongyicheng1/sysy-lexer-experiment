%option noyywrap yylineno
%x COMMENT
%{
#include <stdio.h>

static int lexical_error = 0;
%}

DIGIT       [0-9]
HEX         [0-9a-fA-F]
EXP         [eE][+-]?{DIGIT}+
P_EXP       [pP][+-]?{DIGIT}+
DEC_FLOAT   (({DIGIT}+\.{DIGIT}*)|(\.{DIGIT}+))({EXP})?|{DIGIT}+{EXP}
HEX_FLOAT   0[xX](({HEX}+\.{HEX}*)|(\.{HEX}+)){P_EXP}|0[xX]{HEX}+{P_EXP}
IDENT       [a-zA-Z_][a-zA-Z0-9_]*

%%

"const"     { printf("CONST %s\n", yytext); }
"int"       { printf("INT %s\n", yytext); }
"float"     { printf("FLOAT %s\n", yytext); }
"void"      { printf("VOID %s\n", yytext); }
"break"     { printf("BREAK %s\n", yytext); }
"continue"  { printf("CONTINUE %s\n", yytext); }
"if"        { printf("IF %s\n", yytext); }
"else"      { printf("ELSE %s\n", yytext); }
"while"     { printf("WHILE %s\n", yytext); }
"return"    { printf("RETURN %s\n", yytext); }

{HEX_FLOAT}|{DEC_FLOAT} { printf("FLOAT_CONST %s\n", yytext); }
0[xX]{HEX}+             { printf("INT_CONST %s\n", yytext); }
0[0-7]*                 { printf("INT_CONST %s\n", yytext); }
[1-9]{DIGIT}*           { printf("INT_CONST %s\n", yytext); }
0[0-9]+                 { fprintf(stderr, "ERROR line %d: invalid octal integer %s\n", yylineno, yytext); lexical_error = 1; }

{IDENT}     { printf("ID %s\n", yytext); }

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

"//"[^\n]*  { /* line comment */ }
"/*"        { BEGIN(COMMENT); }
<COMMENT>"*/" { BEGIN(INITIAL); }
<COMMENT>\n  { /* keep line count */ }
<COMMENT>.   { /* skip comment body */ }
<COMMENT><<EOF>> {
    fprintf(stderr, "ERROR line %d: unterminated block comment\n", yylineno);
    lexical_error = 1;
    return 1;
}

[ \t\r\n]+  { /* whitespace */ }

. {
    fprintf(stderr, "ERROR line %d: unrecognized character '%s'\n", yylineno, yytext);
    lexical_error = 1;
}

%%

int main(void) {
    yylex();
    return lexical_error;
}
