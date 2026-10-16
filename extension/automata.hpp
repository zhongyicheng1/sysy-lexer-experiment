#ifndef SYSY_LEXER_AUTOMATA_HPP
#define SYSY_LEXER_AUTOMATA_HPP

#include <map>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace automata {

struct NfaEdge {
    int from;
    int to;
    std::optional<char> symbol; // Empty means epsilon.
};

struct Nfa {
    int start = -1;
    int accept = -1;
    int state_count = 0;
    std::vector<NfaEdge> edges;
};

struct Dfa {
    int start = 0;
    std::set<int> accepting;
    std::vector<std::set<int>> nfa_states;
    std::map<std::pair<int, char>, int> transitions;
};

Nfa regexToNfa(const std::string& regex);
Dfa nfaToDfa(const Nfa& nfa);
void printNfa(const Nfa& nfa);
void printDfa(const Dfa& dfa);

} // namespace automata

#endif
