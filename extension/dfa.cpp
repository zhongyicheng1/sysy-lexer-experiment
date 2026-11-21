#include "dfa.hpp"

#include <map>
#include <queue>
#include <set>
#include <stdexcept>
#include <vector>

namespace automata {
namespace {

std::set<char> alphabetOf(const Dfa& dfa) {
    std::set<char> alphabet;
    for (const auto& [key, target] : dfa.transitions) {
        (void)target;
        alphabet.insert(key.second);
    }
    return alphabet;
}

std::vector<int> reachableStates(const Dfa& dfa) {
    const int state_count = static_cast<int>(dfa.nfa_states.size());
    if (dfa.start < 0 || dfa.start >= state_count) return {};

    std::vector<int> order;
    std::set<int> visited = {dfa.start};
    std::queue<int> pending;
    pending.push(dfa.start);

    while (!pending.empty()) {
        int current = pending.front();
        pending.pop();
        order.push_back(current);

        for (const auto& [key, target] : dfa.transitions) {
            if (key.first == current && target >= 0 && target < state_count &&
                visited.insert(target).second) {
                pending.push(target);
            }
        }
    }
    return order;
}

} // namespace

Dfa minimizeDfa(const Dfa& dfa) {
    const std::vector<int> states = reachableStates(dfa);
    if (states.empty()) {
        throw std::runtime_error("DFA has no reachable start state");
    }

    const std::set<char> alphabet = alphabetOf(dfa);
    std::map<int, int> position;
    for (std::size_t i = 0; i < states.size(); ++i) {
        position[states[i]] = static_cast<int>(i);
    }

    std::vector<int> block(states.size());
    for (std::size_t i = 0; i < states.size(); ++i) {
        block[i] = dfa.accepting.count(states[i]) ? 1 : 0;
    }

    while (true) {
        std::map<std::vector<int>, int> signature_ids;
        std::vector<int> refined(states.size());
        for (std::size_t i = 0; i < states.size(); ++i) {
            std::vector<int> signature = {dfa.accepting.count(states[i]) ? 1 : 0};
            for (char symbol : alphabet) {
                auto transition = dfa.transitions.find({states[i], symbol});
                if (transition == dfa.transitions.end() || !position.count(transition->second)) {
                    signature.push_back(-1);
                } else {
                    signature.push_back(block[position.at(transition->second)]);
                }
            }
            auto [it, inserted] = signature_ids.emplace(
                signature, static_cast<int>(signature_ids.size()));
            (void)inserted;
            refined[i] = it->second;
        }
        if (refined == block) break;
        block.swap(refined);
    }

    int block_count = 0;
    for (int id : block) {
        if (id + 1 > block_count) block_count = id + 1;
    }

    Dfa result;
    result.start = block[position.at(dfa.start)];
    result.nfa_states.resize(static_cast<std::size_t>(block_count));
    for (std::size_t i = 0; i < states.size(); ++i) {
        int new_state = block[i];
        int old_state = states[i];
        if (dfa.accepting.count(old_state)) result.accepting.insert(new_state);
        result.nfa_states[new_state].insert(
            dfa.nfa_states[old_state].begin(), dfa.nfa_states[old_state].end());

        for (char symbol : alphabet) {
            auto transition = dfa.transitions.find({old_state, symbol});
            if (transition != dfa.transitions.end() && position.count(transition->second)) {
                result.transitions[{new_state, symbol}] =
                    block[position.at(transition->second)];
            }
        }
    }
    return result;
}

bool simulateDfa(const Dfa& dfa, const std::string& input) {
    int state = dfa.start;
    if (state < 0 || state >= static_cast<int>(dfa.nfa_states.size())) return false;

    for (char symbol : input) {
        auto transition = dfa.transitions.find({state, symbol});
        if (transition == dfa.transitions.end()) return false;
        state = transition->second;
    }
    return dfa.accepting.count(state) != 0;
}

} // namespace automata
