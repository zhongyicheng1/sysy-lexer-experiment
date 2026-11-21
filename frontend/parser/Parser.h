#pragma once

#include <memory>
#include <string>

#include "../ast/AST.h"

struct ParseResult {
    std::unique_ptr<Program> program;
    std::string error;
    int errorLine = 0;

    bool ok() const noexcept { return program != nullptr && error.empty(); }
};

// Parses the SysY subset needed by the semantic-checking frontend.  The parser
// returns a diagnostic instead of throwing so main.cpp can report all failures
// uniformly and never reach code generation.
ParseResult parseSysY(const std::string& source);
