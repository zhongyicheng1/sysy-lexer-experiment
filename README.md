# SysY lexer experiment

This repository contains a Flex-based SysY lexer and an optional regex-to-automata extension.

## Basic lexer

In WSL/Ubuntu, from the repository root:

```bash
flex -o lexer.c src/lexer.l
gcc lexer.c -o lexer
./lexer < tests/basic.sy
./lexer < tests/edge.sy
```

`tests/lexical_cases.sy` covers additional integer and floating-point forms. `tests/lexical_errors.sy` intentionally contains invalid input and should print an error and return a nonzero status.

The lexer currently prints token category and lexeme. Token attributes, identifier-table indexes, and symbol-table lookup are the next integration module.

## Automata extension

See [extension/README.md](extension/README.md) for regex syntax, build instructions, and the NFA/DFA interface.
