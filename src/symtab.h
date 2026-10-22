#ifndef SYSY_SYMTAB_H
#define SYSY_SYMTAB_H

#ifdef __cplusplus
extern "C" {
#endif

#define SYMTAB_MAX 1000

/*
 * 名字驻留（interning）：
 *   已存在 -> 返回旧下标；
 *   不存在 -> strdup 后追加，返回新下标。
 * 表满或 name == NULL 时返回 -1。
 */
int symtab_intern(const char *name);

/* 只查不插：存在返回下标，否则 -1。 */
int symtab_lookup(const char *name);

int symtab_count(void);

/* 释放所有驻留字符串。程序退出前调用一次即可；
 * 调用后表被清空，可继续使用。 */
void symtab_destroy(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSY_SYMTAB_H */
