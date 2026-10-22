/* strdup 是 POSIX 函数：在 -std=c11/-std=c99 下显式声明，
 * 避免 gcc 报 implicit declaration（C99 起这已是错误）。
 * 必须写在所有 #include 之前。 */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *token_kind_name(TokenKind kind)
{
    switch (kind) {
    case TOK_EOF:         return "TOK_EOF";
    case TOK_CONST:       return "TOK_CONST";
    case TOK_INT:         return "TOK_INT";
    case TOK_FLOAT:       return "TOK_FLOAT";
    case TOK_VOID:        return "TOK_VOID";
    case TOK_BREAK:       return "TOK_BREAK";
    case TOK_CONTINUE:    return "TOK_CONTINUE";
    case TOK_IF:          return "TOK_IF";
    case TOK_ELSE:        return "TOK_ELSE";
    case TOK_WHILE:       return "TOK_WHILE";
    case TOK_RETURN:      return "TOK_RETURN";
    case TOK_ID:          return "TOK_ID";
    case TOK_INT_CONST:   return "TOK_INT_CONST";
    case TOK_FLOAT_CONST: return "TOK_FLOAT_CONST";
    case TOK_EQ:          return "TOK_EQ";
    case TOK_NE:          return "TOK_NE";
    case TOK_LE:          return "TOK_LE";
    case TOK_GE:          return "TOK_GE";
    case TOK_AND:         return "TOK_AND";
    case TOK_OR:          return "TOK_OR";
    case TOK_NOT:         return "TOK_NOT";
    case TOK_PLUS:        return "TOK_PLUS";
    case TOK_MINUS:       return "TOK_MINUS";
    case TOK_MUL:         return "TOK_MUL";
    case TOK_DIV:         return "TOK_DIV";
    case TOK_MOD:         return "TOK_MOD";
    case TOK_ASSIGN:      return "TOK_ASSIGN";
    case TOK_LT:          return "TOK_LT";
    case TOK_GT:          return "TOK_GT";
    case TOK_LPAREN:      return "TOK_LPAREN";
    case TOK_RPAREN:      return "TOK_RPAREN";
    case TOK_LBRACKET:    return "TOK_LBRACKET";
    case TOK_RBRACKET:    return "TOK_RBRACKET";
    case TOK_LBRACE:      return "TOK_LBRACE";
    case TOK_RBRACE:      return "TOK_RBRACE";
    case TOK_COMMA:       return "TOK_COMMA";
    case TOK_SEMICOLON:   return "TOK_SEMICOLON";
    case TOK_ERROR:       return "TOK_ERROR";
    }
    return "TOK_UNKNOWN";
}

Token make_token(TokenKind kind, const char *text, int line)
{
    Token tok;

    tok.kind = kind;
    tok.lexeme = strdup(text != NULL ? text : "");
    if (tok.lexeme == NULL) {
        fprintf(stderr, "fatal: out of memory while copying a lexeme\n");
        exit(EXIT_FAILURE);
    }
    tok.line = line;
    tok.sym_index = -1;
    tok.value.ival = 0;          /* 同时把 fval 清成 0.0 */
    return tok;
}

void token_free(Token *tok)
{
    if (tok == NULL) {
        return;
    }
    free(tok->lexeme);
    tok->lexeme = NULL;
    tok->line = 0;
    tok->sym_index = -1;
    tok->value.ival = 0;
}

void token_print(const Token *tok)
{
    const char *text;

    if (tok == NULL) {
        return;
    }
    text = (tok->lexeme != NULL) ? tok->lexeme : "";

    switch (tok->kind) {
    case TOK_INT_CONST:
        printf("<%s, \"%s\", line=%d, sym=%d, val=%ld>\n",
               token_kind_name(tok->kind), text,
               tok->line, tok->sym_index, tok->value.ival);
        break;
    case TOK_FLOAT_CONST:
        printf("<%s, \"%s\", line=%d, sym=%d, val=%g>\n",
               token_kind_name(tok->kind), text,
               tok->line, tok->sym_index, tok->value.fval);
        break;
    default:
        printf("<%s, \"%s\", line=%d, sym=%d, val=0>\n",
               token_kind_name(tok->kind), text,
               tok->line, tok->sym_index);
        break;
    }
}
