#include "automata.hpp"

#include <algorithm>
#include <iostream>
#include <queue>
#include <stdexcept>

namespace automata {
namespace {

struct Fragment {
    int start;
    int accept;
};

class RegexParser {
public:
    explicit RegexParser(const std::string& input) : input_(input) {}

    Nfa parse() {
        if (input_.empty()) {
            throw std::runtime_error("regular expression must not be empty");
        }
        Fragment result = parseAlternation();
        if (position_ != input_.size()) {
            fail("unexpected character");
        }
        nfa_.start = result.start;
        nfa_.accept = result.accept;
        return nfa_;
    }

private:
    const std::string& input_;
    std::size_t position_ = 0;
    Nfa nfa_;

    [[noreturn]] void fail(const std::string& message) const {
        throw std::runtime_error(message + " at position " + std::to_string(position_));
    }

    int state() {
        return nfa_.state_count++;
    }

    void edge(int from, int to, std::optional<char> symbol = std::nullopt) {
        nfa_.edges.push_back({from, to, symbol});
    }

    Fragment literal(char symbol) {
        int from = state();
        int to = state();
        edge(from, to, symbol);
        return {from, to};
    }

    Fragment concatenate(Fragment left, Fragment right) {
        edge(left.accept, right.start);
        return {left.start, right.accept};
    }

    Fragment alternate(Fragment left, Fragment right) {
        int start = state();
        int accept = state();
        edge(start, left.start);
        edge(start, right.start);
        edge(left.accept, accept);
        edge(right.accept, accept);
        return {start, accept};
    }

    Fragment parseAlternation() {
        Fragment result = parseConcatenation();
        while (take('|')) {
            result = alternate(result, parseConcatenation());
        }
        return result;
    }

    Fragment parseConcatenation() {
        if (atEnd() || peek() == ')' || peek() == '|') {
            fail("expected an expression");
        }
        Fragment result = parseRepetition();
        while (!atEnd() && peek() != ')' && peek() != '|') {
            result = concatenate(result, parseRepetition());
        }
        return result;
    }

    Fragment parseRepetition() {
        Fragment result = parseAtom();
        while (!atEnd()) {
            if (take('*')) {
                int start = state();
                int accept = state();
                edge(start, result.start);
                edge(start, accept);
                edge(result.accept, result.start);
                edge(result.accept, accept);
                result = {start, accept};
            } else if (take('+')) {
                int accept = state();
                edge(result.accept, result.start);
                edge(result.accept, accept);
                result.accept = accept;
            } else if (take('?')) {
                int start = state();
                int accept = state();
                edge(start, result.start);
                edge(start, accept);
                edge(result.accept, accept);
                result = {start, accept};
            } else {
                break;
            }
        }
        return result;
    }

    char escapedCharacter() {
        if (atEnd()) {
            fail("unfinished escape sequence");
        }
        char value = input_[position_++];
        switch (value) {
        case 'n': return '\n';
        case 'r': return '\r';
        case 't': return '\t';
        default: return value;
        }
    }

    std::pair<char, bool> classCharacter() {
        if (atEnd() || peek() == ']') {
            fail("expected character in character class");
        }
        if (take('\\')) {
            return {escapedCharacter(), true};
        }
        return {input_[position_++], false};
    }

    Fragment parseCharacterClass() {
        ++position_; // '['
        if (!atEnd() && peek() == '^') {
            fail("negated character classes are not supported");
        }
        std::vector<char> members;
        while (!atEnd() && peek() != ']') {
            auto first = classCharacter();
            if (!atEnd() && peek() == '-' && position_ + 1 < input_.size() && input_[position_ + 1] != ']') {
                ++position_;
                auto last = classCharacter();
                if (first.first > last.first) {
                    fail("reversed range in character class");
                }
                for (unsigned int c = static_cast<unsigned char>(first.first);
                     c <= static_cast<unsigned char>(last.first); ++c) {
                    members.push_back(static_cast<char>(c));
                }
            } else {
                members.push_back(first.first);
            }
        }
        if (atEnd()) {
            fail("unclosed character class");
        }
        ++position_; // ']'
        if (members.empty()) {
            fail("empty character class");
        }
        std::sort(members.begin(), members.end());
        members.erase(std::unique(members.begin(), members.end()), members.end());
        Fragment result = literal(members.front());
        for (std::size_t i = 1; i < members.size(); ++i) {
            result = alternate(result, literal(members[i]));
        }
        return result;
    }

    Fragment parseAtom() {
        if (atEnd()) {
            fail("expected an atom");
        }
        if (take('(')) {
            Fragment nested = parseAlternation();
            if (!take(')')) {
                fail("missing closing parenthesis");
            }
            return nested;
        }
        if (peek() == '[') {
            return parseCharacterClass();
        }
        if (take('\\')) {
            return literal(escapedCharacter());
        }
        char value = input_[position_++];
        if (value == ')' || value == '|' || value == '*' || value == '+' || value == '?') {
            fail("operator has no left operand");
        }
        return literal(value);
    }

    bool atEnd() const { return position_ >= input_.size(); }
    char peek() const { return input_[position_]; }
    bool take(char value) {
        if (!atEnd() && peek() == value) {
            ++position_;
            return true;
        }
        return false;
    }
};

std::set<int> epsilonClosure(const Nfa& nfa, const std::set<int>& seeds) {
    std::set<int> closure = seeds;
    std::vector<int> pending(seeds.begin(), seeds.end());
    while (!pending.empty()) {
        int current = pending.back();
        pending.pop_back();
        for (const auto& transition : nfa.edges) {
            if (transition.from == current && !transition.symbol && closure.insert(transition.to).second) {
                pending.push_back(transition.to);
            }
        }
    }
    return closure;
}

} // namespace

Nfa regexToNfa(const std::string& regex) {
    return RegexParser(regex).parse();
}

Dfa nfaToDfa(const Nfa& nfa) {
    if (nfa.start < 0 || nfa.accept < 0) {
        throw std::runtime_error("NFA has no start or accepting state");
    }
    std::set<char> alphabet;
    for (const auto& transition : nfa.edges) {
        if (transition.symbol) {
            alphabet.insert(*transition.symbol);
        }
    }

    Dfa dfa;
    std::map<std::set<int>, int> state_ids;
    std::queue<int> pending;
    std::set<int> start_set = epsilonClosure(nfa, {nfa.start});
    state_ids[start_set] = 0;
    dfa.nfa_states.push_back(start_set);
    pending.push(0);

    while (!pending.empty()) {
        int current_id = pending.front();
        pending.pop();
        const std::set<int> current = dfa.nfa_states[current_id];

        if (current.count(nfa.accept)) {
            dfa.accepting.insert(current_id);
        }

        for (char symbol : alphabet) {
            std::set<int> moved;
            for (const auto& transition : nfa.edges) {
                if (transition.symbol == symbol && current.count(transition.from)) {
                    moved.insert(transition.to);
                }
            }
            if (moved.empty()) {
                continue;
            }
            std::set<int> next = epsilonClosure(nfa, moved);
            auto [it, inserted] = state_ids.emplace(next, static_cast<int>(state_ids.size()));
            if (inserted) {
                dfa.nfa_states.push_back(next);
                pending.push(it->second);
            }
            dfa.transitions[{current_id, symbol}] = it->second;
        }
    }
    return dfa;
}

void printNfa(const Nfa& nfa) {
    std::cout << "NFA start: " << nfa.start << "\nNFA accept: " << nfa.accept << '\n';
    for (const auto& transition : nfa.edges) {
        std::cout << transition.from << " --";
        if (transition.symbol) {
            if (*transition.symbol == '\n') std::cout << "\\n";
            else if (*transition.symbol == '\t') std::cout << "\\t";
            else std::cout << *transition.symbol;
        } else {
            std::cout << "eps";
        }
        std::cout << "--> " << transition.to << '\n';
    }
}

void printDfa(const Dfa& dfa) {
    std::cout << "DFA start: " << dfa.start << "\nDFA accepting:";
    for (int state : dfa.accepting) std::cout << ' ' << state;
    std::cout << "\nDFA states (id = NFA-state-set):\n";
    for (std::size_t id = 0; id < dfa.nfa_states.size(); ++id) {
        std::cout << "  " << id << " = {";
        bool first = true;
        for (int state : dfa.nfa_states[id]) {
            if (!first) std::cout << ',';
            std::cout << state;
            first = false;
        }
        std::cout << "}" << (dfa.accepting.count(static_cast<int>(id)) ? " [accept]" : "") << '\n';
    }
    std::cout << "DFA transitions:\n";
    for (const auto& [key, target] : dfa.transitions) {
        std::cout << "  " << key.first << " --" << key.second << "--> " << target << '\n';
    }
}

} // namespace automata

#ifndef AUTOMATA_NO_MAIN
int main(int argc, char** argv) {
    try {
        std::string regex;
        if (argc > 1) {
            regex = argv[1];
        } else {
            std::cout << "Regex: ";
            std::getline(std::cin, regex);
        }
        automata::Nfa nfa = automata::regexToNfa(regex);
        automata::Dfa dfa = automata::nfaToDfa(nfa);
        automata::printNfa(nfa);
        std::cout << '\n';
        automata::printDfa(dfa);
    } catch (const std::exception& error) {
        std::cerr << "Regex error: " << error.what() << '\n';
        return 1;
    }
}
#endif
