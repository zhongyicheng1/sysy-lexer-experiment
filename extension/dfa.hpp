#ifndef SYSY_LEXER_DFA_HPP
#define SYSY_LEXER_DFA_HPP

#include "automata.hpp"

#include <string>

namespace automata {

Dfa minimizeDfa(const Dfa& dfa);
bool simulateDfa(const Dfa& dfa, const std::string& input);

} // namespace automata

#endif
