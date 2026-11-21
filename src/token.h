#ifndef SYSY_TOKEN_H
#define SYSY_TOKEN_H

#ifdef __cplusplus
extern "C" {
#endif

/* 全量 SysY 记号。TOK_EOF 必须为 0，便于 while (yylex() != 0)。 */
typedef enum {
    TOK_EOF = 0,

    /* 关键字 */
    TOK_CONST, TOK_INT, TOK_FLOAT, TOK_VOID,
    TOK_BREAK, TOK_CONTINUE, TOK_IF, TOK_ELSE, TOK_WHILE, TOK_RETURN,

    /* 标识符与常量 */
    TOK_ID, TOK_INT_CONST, TOK_FLOAT_CONST,

    /* 运算符 */
    TOK_EQ, TOK_NE, TOK_LE, TOK_GE, TOK_AND, TOK_OR, TOK_NOT,
    TOK_PLUS, TOK_MINUS, TOK_MUL, TOK_DIV, TOK_MOD,
    TOK_ASSIGN, TOK_LT, TOK_GT,

    /* 分隔符 */
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACKET, TOK_RBRACKET,
    TOK_LBRACE, TOK_RBRACE, TOK_COMMA, TOK_SEMICOLON,

    /* 词法错误：非法字符、非法八进制、未闭合注释 */
    TOK_ERROR
} TokenKind;

/*
 * lexeme 是堆上的独立副本（make_token 里 strdup），
 * 因此 Token 的生命周期与 yytext 无关。
 * sym_index 仅对 TOK_ID 有意义，其余为 -1。
 */
typedef struct {
    TokenKind kind;
    char     *lexeme;
    int       line;       /* 从 1 开始，取自 yylineno */
    int       sym_index;  /* 符号表下标；非标识符为 -1 */
    union {
        long   ival;
        double fval;
    } value;
} Token;

const char *token_kind_name(TokenKind kind);

/* 复制 text（通常是 yytext），失败直接终止程序，绝不返回半残 Token。 */
Token make_token(TokenKind kind, const char *text, int line);

/* 释放 lexeme 并复位字段，重复调用安全。 */
void token_free(Token *tok);

/* 形如：<TOK_ID, "a", line=1, sym=0, val=0> */
void token_print(const Token *tok);

#ifdef __cplusplus
}
#endif

#endif /* SYSY_TOKEN_H */
