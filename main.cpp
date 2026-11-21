#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "frontend/ast/visitor/SemanticCheck.h"
#include "frontend/parser/Parser.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source.sy>\n";
        return 1;
    }

    std::ifstream input(argv[1], std::ios::binary);
    if (!input) {
        std::cerr << "[Line 0][IOError] Cannot open input file '" << argv[1]
                  << "'\n";
        return 1;
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    if (input.bad()) {
        std::cerr << "[Line 0][IOError] Failed to read input file '"
                  << argv[1] << "'\n";
        return 1;
    }

    const ParseResult parsed = parseSysY(buffer.str());
    if (!parsed.ok()) {
        if (!parsed.error.empty()) {
            std::cerr << "[Line " << parsed.errorLine
                      << "][SyntaxError] " << parsed.error << '\n';
        } else {
            std::cerr << "[Line 0][InternalError] Parser produced no AST\n";
        }
        return 1;
    }

    SemanticCheck checker;
    if (!checker.check(parsed.program.get())) {
        for (const std::string& error : checker.getErrors()) {
            std::cerr << error << '\n';
        }
        return 1;
    }

    // Code generation may only begin after the semantic check succeeds.
    return 0;
}
