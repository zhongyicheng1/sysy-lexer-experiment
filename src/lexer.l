%option noyywrap yylineno nounput noinput

%x COMMENT

%{
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "symtab.h"

/*
 * 所有动作的统一出口：
 *   1. 用 strdup 复制 yytext（Token 的生命周期与 lexer 缓冲区解耦）；
 *   2. 标识符额外查/插符号表，得到 sym_index；
 *   3. 打印并立即释放，避免逐 token 泄漏。
 * 用 do { ... } while (0) 包裹，保证宏在 if/else 中作为单条语句使用。
 */
#define EMIT(k) do {                                              \
        Token _t = make_token((k), yytext, yylineno);             \
        if ((k) == TOK_ID) {                                      \
            _t.sym_index = symtab_intern(_t.lexeme);              \
        }                                                         \
        token_print(&_t);                                         \
        token_free(&_t);                                          \
    } while (0)

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

"const"     { EMIT(TOK_CONST); }
"int"       { EMIT(TOK_INT); }
"float"     { EMIT(TOK_FLOAT); }
"void"      { EMIT(TOK_VOID); }
"break"     { EMIT(TOK_BREAK); }
"continue"  { EMIT(TOK_CONTINUE); }
"if"        { EMIT(TOK_IF); }
"else"      { EMIT(TOK_ELSE); }
"while"     { EMIT(TOK_WHILE); }
"return"    { EMIT(TOK_RETURN); }

{HEX_FLOAT}|{DEC_FLOAT} {
    /* 常量：先识别形状，再在动作里把字面量求值进 value。
     * 浮点规则必须排在整数规则之前（最长匹配之外，同类前缀靠顺序兜底）。 */
    Token t = make_token(TOK_FLOAT_CONST, yytext, yylineno);
    t.value.fval = strtod(yytext, NULL);   /* C99 起支持 0x1.8p3 */
    token_print(&t);
    token_free(&t);
}

0[xX]{HEX}+ {
    Token t = make_token(TOK_INT_CONST, yytext, yylineno);
    t.value.ival = strtol(yytext, NULL, 0); /* base=0 自动识别 0x */
    token_print(&t);
    token_free(&t);
}

0[0-7]* {
    Token t = make_token(TOK_INT_CONST, yytext, yylineno);
    t.value.ival = strtol(yytext, NULL, 0); /* 前导 0 按八进制 */
    token_print(&t);
    token_free(&t);
}

[1-9]{DIGIT}* {
    Token t = make_token(TOK_INT_CONST, yytext, yylineno);
    t.value.ival = strtol(yytext, NULL, 10);
    token_print(&t);
    token_free(&t);
}

0[0-9]+ {
    /* 0 后跟 8/9：非法八进制。先报 stderr，再产出 TOK_ERROR 保持 token 流完整。 */
    fprintf(stderr, "ERROR line %d: invalid octal integer '%s'\n",
            yylineno, yytext);
    EMIT(TOK_ERROR);
    lexical_error = 1;
}

{IDENT} {
    EMIT(TOK_ID);
}

"=="        { EMIT(TOK_EQ); }
"!="        { EMIT(TOK_NE); }
"<="        { EMIT(TOK_LE); }
">="        { EMIT(TOK_GE); }
"&&"        { EMIT(TOK_AND); }
"||"        { EMIT(TOK_OR); }

"+"         { EMIT(TOK_PLUS); }
"-"         { EMIT(TOK_MINUS); }
"*"         { EMIT(TOK_MUL); }
"/"         { EMIT(TOK_DIV); }
"%"         { EMIT(TOK_MOD); }
"="         { EMIT(TOK_ASSIGN); }
"<"         { EMIT(TOK_LT); }
">"         { EMIT(TOK_GT); }
"!"         { EMIT(TOK_NOT); }

"("         { EMIT(TOK_LPAREN); }
")"         { EMIT(TOK_RPAREN); }
"["         { EMIT(TOK_LBRACKET); }
"]"         { EMIT(TOK_RBRACKET); }
"{"         { EMIT(TOK_LBRACE); }
"}"         { EMIT(TOK_RBRACE); }
","         { EMIT(TOK_COMMA); }
";"         { EMIT(TOK_SEMICOLON); }

"//"[^\n]*    { /* 行注释：整段丢弃 */ }
"/*"          { BEGIN(COMMENT); }

<COMMENT>"*/" { BEGIN(INITIAL); }
<COMMENT>\n   { /* flex 的 yylineno 会自动加一 */ }
<COMMENT>.    { /* 注释体丢弃 */ }

<COMMENT><<EOF>> {
    Token t = make_token(TOK_ERROR, "unterminated comment", yylineno);
    fprintf(stderr, "ERROR line %d: unterminated block comment\n", yylineno);
    token_print(&t);
    token_free(&t);
    lexical_error = 1;
    yyterminate();
}

[ \t\r\n]+  { /* 空白：含换行，yylineno 自动维护 */ }

. {
    fprintf(stderr, "ERROR line %d: unrecognized character '%s'\n",
            yylineno, yytext);
    EMIT(TOK_ERROR);
    lexical_error = 1;
}

<<EOF>> {
    EMIT(TOK_EOF);
    yyterminate();
}

%%

int main(void)
{
    /* 动作里已经完成打印/释放；这里只驱动扫描直到 0。 */
    while (yylex() != 0) {
        /* 未来接 parser 时，把 EMIT 换成 return 即可，此循环不用改 */
    }
    yylex_destroy();   /* 释放 flex 内部缓冲区，配合 ASan 可做到零泄漏 */
    symtab_destroy();
    return lexical_error;
}
