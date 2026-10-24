# Lex algorithm extension

This directory demonstrates the complete automata pipeline:

```text
regular expression -> NFA -> DFA -> minimized DFA -> string simulation
```

## Supported regular expressions

- Literal characters and escapes such as `\|`, `\*`, `\n`, `\r`, and `\t`
- Concatenation, alternation (`|`), repetition (`*`, `+`, `?`), and parentheses
- Character classes and ranges such as `[a-zA-Z_]`

Negated character classes are not supported. A period is treated as a literal period.

## Build and run

From the repository root:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic \
  extension/automata.cpp extension/dfa.cpp -o extension/automata

./extension/automata '(a|b)*abb' abb aabb ab aba b
```

The program prints the Thompson NFA, the DFA produced by subset construction, the minimized DFA, and the acceptance result for every supplied string. It also checks that minimization does not change each result.

## Module interfaces

`automata.hpp` provides regular-expression parsing, NFA construction, and subset construction. `dfa.hpp` provides:

```cpp
automata::Dfa minimized = automata::minimizeDfa(dfa);
bool accepted = automata::simulateDfa(minimized, input);
```

Both stages use the same `automata::Dfa` structure, so the minimized and simulated automaton is the DFA generated from the input regular expression rather than a separately hard-coded example.
