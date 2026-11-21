# SysY lexer experiment

This repository contains a Flex-based SysY lexer and a complete regex-to-automata extension.

## Basic lexer

In WSL/Ubuntu, from the repository root:

```bash
flex -o lexer.c src/lexer.l
gcc -std=gnu11 -Isrc lexer.c src/token.c src/symtab.c -o lexer
./lexer < tests/basic.sy
./lexer < tests/edge.sy
```

`tests/lexical_cases.sy` covers additional integer and floating-point forms. `tests/lexical_errors.sy` intentionally contains invalid input and should print an error and return a nonzero status.

Each token is printed on its own line, for example `<TOK_INT_CONST, "10", line=3, sym=-1, val=10>`.
Identifiers are interned in the symbol table and carry a `sym_index`; lexical errors are emitted as
`TOK_ERROR` records on stdout with diagnostics on stderr, and the process exits with status 1.

## Automata extension

See [extension/README.md](extension/README.md) for the integrated regex, NFA, DFA, minimization, and simulation workflow.

## AI usage

The required AI assistance record is in [AI_Usage_Log.md](AI_Usage_Log.md).
