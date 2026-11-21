/* strdup 的 POSIX 声明，必须位于所有 #include 之前。 */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include "symtab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 极简符号表：定长指针数组 + 线性查找。零初始化。 */
static char *table[SYMTAB_MAX];
static int count = 0;

int symtab_lookup(const char *name)
{
    int i;

    if (name == NULL) {
        return -1;
    }
    for (i = 0; i < count; ++i) {
        if (strcmp(table[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

int symtab_intern(const char *name)
{
    int idx;
    char *copy;

    if (name == NULL) {
        return -1;
    }
    idx = symtab_lookup(name);
    if (idx >= 0) {
        return idx;
    }
    if (count >= SYMTAB_MAX) {
        fprintf(stderr, "warning: symbol table is full (%d entries), "
                        "identifier '%s' is not interned\n", SYMTAB_MAX, name);
        return -1;
    }
    copy = strdup(name);
    if (copy == NULL) {
        fprintf(stderr, "fatal: out of memory while interning a symbol\n");
        exit(EXIT_FAILURE);
    }
    table[count] = copy;
    return count++;
}

int symtab_count(void)
{
    return count;
}

void symtab_destroy(void)
{
    int i;

    for (i = 0; i < count; ++i) {
        free(table[i]);
        table[i] = NULL;
    }
    count = 0;
}
