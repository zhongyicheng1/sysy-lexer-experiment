# Regex to NFA to DFA

This extension implements the part assigned to Zhong Yicheng: parse a regular expression, build a Thompson NFA, and convert it to a DFA with epsilon-closure and subset construction. DFA minimization and input simulation are left for the next module.

## Supported syntax

- Literals, including escaped metacharacters such as `\|` and `\*`
- Concatenation by writing expressions next to each other
- Alternation: `a|b`
- Repetition: `*`, `+`, and `?`
- Parentheses: `(ab|c)*`
- Character classes and ranges: `[a-zA-Z_]`
- Escapes: `\n`, `\r`, `\t`, or a backslash before a literal character

Negated character classes are not supported. A period is a literal period.

## Build and run

From the repository root:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic extension/automata.cpp -o extension/automata
./extension/automata '(a|b)*abb'
```

The program prints the NFA edge list followed by the DFA states and transition table. Each DFA state's `nfa_states` set records its subset-construction origin.

## Interface for the next module

Include `automata.hpp` and use:

```cpp
automata::Nfa nfa = automata::regexToNfa("(a|b)*abb");
automata::Dfa dfa = automata::nfaToDfa(nfa);
```

`Dfa` exposes `start`, `accepting`, `nfa_states`, and `transitions`. A missing `(state, character)` transition means rejection for that input step. Define `AUTOMATA_NO_MAIN` when compiling `automata.cpp` as a library with another program's `main`.
