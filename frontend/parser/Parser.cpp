#include "Parser.h"

#include <cstddef>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace {

struct ParseError {
    int line;
    std::string message;
};

[[noreturn]] void fail(int line, const std::string& message) {
    throw ParseError{line, message};
}

enum class TokenKind {
    End,
    Identifier,
    IntLiteral,

    KwInt,
    KwBool,
    KwVoid,
    KwConst,
    KwIf,
    KwElse,
    KwWhile,
    KwReturn,
    KwTrue,
    KwFalse,

    LParen,
    RParen,
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    Comma,
    Semicolon,

    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Assign,
    Eq,
    Ne,
    Lt,
    Gt,
    Le,
    Ge,
    AndAnd,
    OrOr,
    Bang,
};

struct Token {
    TokenKind kind = TokenKind::End;
    std::string text;
    int line = 1;
    int intValue = 0;
};

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isHexDigit(char c) {
    return isDigit(c) || (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}

bool isIdentifierStart(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool isIdentifierPart(char c) {
    return isIdentifierStart(c) || isDigit(c);
}

int hexValue(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return c - 'A' + 10;
}

TokenKind keywordKind(const std::string& text) {
    if (text == "int") {
        return TokenKind::KwInt;
    }
    if (text == "bool") {
        return TokenKind::KwBool;
    }
    if (text == "void") {
        return TokenKind::KwVoid;
    }
    if (text == "const") {
        return TokenKind::KwConst;
    }
    if (text == "if") {
        return TokenKind::KwIf;
    }
    if (text == "else") {
        return TokenKind::KwElse;
    }
    if (text == "while") {
        return TokenKind::KwWhile;
    }
    if (text == "return") {
        return TokenKind::KwReturn;
    }
    if (text == "true") {
        return TokenKind::KwTrue;
    }
    if (text == "false") {
        return TokenKind::KwFalse;
    }
    return TokenKind::Identifier;
}

class Lexer {
public:
    explicit Lexer(const std::string& source) : source_(source) {}

    std::vector<Token> tokenize() {
        skipBom();
        std::vector<Token> tokens;

        while (!atEnd()) {
            skipIgnored();
            if (atEnd()) {
                break;
            }

            const int line = line_;
            const char c = peek();

            if (isIdentifierStart(c)) {
                tokens.push_back(scanIdentifier(line));
            } else if (isDigit(c)) {
                tokens.push_back(scanNumber(line));
            } else {
                tokens.push_back(scanPunctuation(line));
            }
        }

        tokens.push_back(Token{TokenKind::End, "", line_, 0});
        return tokens;
    }

private:
    bool atEnd() const { return pos_ >= source_.size(); }

    char peek(std::size_t offset = 0) const {
        const std::size_t index = pos_ + offset;
        return index < source_.size() ? source_[index] : '\0';
    }

    char advance() {
        if (atEnd()) {
            return '\0';
        }
        const char c = source_[pos_++];
        if (c == '\n') {
            ++line_;
        }
        return c;
    }

    void skipBom() {
        if (source_.size() >= 3 &&
            static_cast<unsigned char>(source_[0]) == 0xEF &&
            static_cast<unsigned char>(source_[1]) == 0xBB &&
            static_cast<unsigned char>(source_[2]) == 0xBF) {
            pos_ = 3;
        }
    }

    void skipIgnored() {
        for (;;) {
            while (!atEnd()) {
                const char c = peek();
                if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                    advance();
                } else {
                    break;
                }
            }

            if (peek() == '/' && peek(1) == '/') {
                while (!atEnd() && peek() != '\n') {
                    advance();
                }
                continue;
            }

            if (peek() == '/' && peek(1) == '*') {
                const int startLine = line_;
                advance();
                advance();
                while (!atEnd() && !(peek() == '*' && peek(1) == '/')) {
                    advance();
                }
                if (atEnd()) {
                    fail(startLine, "unterminated block comment");
                }
                advance();
                advance();
                continue;
            }

            break;
        }
    }

    Token scanIdentifier(int line) {
        const std::size_t start = pos_;
        while (!atEnd() && isIdentifierPart(peek())) {
            advance();
        }
        const std::string text = source_.substr(start, pos_ - start);
        return Token{keywordKind(text), text, line, 0};
    }

    Token scanNumber(int line) {
        int value = 0;
        bool overflow = false;

        if (peek() == '0' && (peek(1) == 'x' || peek(1) == 'X')) {
            advance();
            advance();
            int digits = 0;
            while (!atEnd() && isHexDigit(peek())) {
                const int digit = hexValue(peek());
                if (value > (std::numeric_limits<int>::max() - digit) / 16) {
                    overflow = true;
                } else {
                    value = value * 16 + digit;
                }
                ++digits;
                advance();
            }
            if (digits == 0) {
                fail(line, "hexadecimal literal has no digits");
            }
        } else {
            while (!atEnd() && isDigit(peek())) {
                const int digit = peek() - '0';
                if (value > (std::numeric_limits<int>::max() - digit) / 10) {
                    overflow = true;
                } else {
                    value = value * 10 + digit;
                }
                advance();
            }
        }

        if (overflow) {
            fail(line, "integer literal is too large");
        }
        return Token{TokenKind::IntLiteral, "", line, value};
    }

    Token scanPunctuation(int line) {
        const char c = advance();
        switch (c) {
            case '(':
                return Token{TokenKind::LParen, "(", line, 0};
            case ')':
                return Token{TokenKind::RParen, ")", line, 0};
            case '{':
                return Token{TokenKind::LBrace, "{", line, 0};
            case '}':
                return Token{TokenKind::RBrace, "}", line, 0};
            case '[':
                return Token{TokenKind::LBracket, "[", line, 0};
            case ']':
                return Token{TokenKind::RBracket, "]", line, 0};
            case ',':
                return Token{TokenKind::Comma, ",", line, 0};
            case ';':
                return Token{TokenKind::Semicolon, ";", line, 0};
            case '+':
                return Token{TokenKind::Plus, "+", line, 0};
            case '-':
                return Token{TokenKind::Minus, "-", line, 0};
            case '*':
                return Token{TokenKind::Star, "*", line, 0};
            case '/':
                return Token{TokenKind::Slash, "/", line, 0};
            case '%':
                return Token{TokenKind::Percent, "%", line, 0};
            case '=':
                if (peek() == '=') {
                    advance();
                    return Token{TokenKind::Eq, "==", line, 0};
                }
                return Token{TokenKind::Assign, "=", line, 0};
            case '!':
                if (peek() == '=') {
                    advance();
                    return Token{TokenKind::Ne, "!=", line, 0};
                }
                return Token{TokenKind::Bang, "!", line, 0};
            case '<':
                if (peek() == '=') {
                    advance();
                    return Token{TokenKind::Le, "<=", line, 0};
                }
                return Token{TokenKind::Lt, "<", line, 0};
            case '>':
                if (peek() == '=') {
                    advance();
                    return Token{TokenKind::Ge, ">=", line, 0};
                }
                return Token{TokenKind::Gt, ">", line, 0};
            case '&':
                if (peek() == '&') {
                    advance();
                    return Token{TokenKind::AndAnd, "&&", line, 0};
                }
                fail(line, "expected '&&'");
            case '|':
                if (peek() == '|') {
                    advance();
                    return Token{TokenKind::OrOr, "||", line, 0};
                }
                fail(line, "expected '||'");
            default:
                fail(line, "unexpected character '" + std::string(1, c) + "'");
        }
    }

    const std::string& source_;
    std::size_t pos_ = 0;
    int line_ = 1;
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

    std::unique_ptr<Program> parseProgram() {
        auto program = std::make_unique<Program>(1);

        while (!check(TokenKind::End)) {
            if (!isDeclStart()) {
                fail(peek(), "expected a top-level declaration or function");
            }

            const DeclSpec spec = parseDeclSpec();
            const Token name = expect(TokenKind::Identifier, "identifier");

            if (check(TokenKind::LParen)) {
                if (spec.isConst) {
                    fail(name, "function return type cannot be const");
                }
                program->functions.push_back(parseFunction(spec, name));
            } else {
                std::vector<std::unique_ptr<VarDecl>> declarations;
                parseVariableDeclarators(spec, name, declarations);
                for (auto& declaration : declarations) {
                    program->globals.push_back(std::move(declaration));
                }
            }
        }

        return program;
    }

private:
    struct DeclSpec {
        BaseType base = BaseType::INT;
        bool isConst = false;
    };

    bool check(TokenKind kind) const { return peek().kind == kind; }

    const Token& peek(std::size_t offset = 0) const {
        const std::size_t index = index_ + offset;
        return tokens_[index < tokens_.size() ? index : tokens_.size() - 1];
    }

    const Token& advance() {
        const Token& token = peek();
        if (!check(TokenKind::End)) {
            ++index_;
        }
        return token;
    }

    bool match(TokenKind kind) {
        if (!check(kind)) {
            return false;
        }
        advance();
        return true;
    }

    const Token& expect(TokenKind kind, const std::string& what) {
        if (!check(kind)) {
            fail(peek(), "expected " + what);
        }
        return advance();
    }

    [[noreturn]] void fail(const Token& token,
                           const std::string& message) const {
        throw ParseError{token.line, message};
    }

    bool isDeclStart() const {
        return check(TokenKind::KwInt) || check(TokenKind::KwBool) ||
               check(TokenKind::KwVoid) || check(TokenKind::KwConst);
    }

    DeclSpec parseDeclSpec() {
        DeclSpec spec;
        spec.isConst = match(TokenKind::KwConst);

        if (match(TokenKind::KwInt)) {
            spec.base = BaseType::INT;
        } else if (match(TokenKind::KwBool)) {
            spec.base = BaseType::BOOL;
        } else if (match(TokenKind::KwVoid)) {
            spec.base = BaseType::VOID;
        } else {
            fail(peek(), "expected a type");
        }
        return spec;
    }

    Type parseDeclaratorType(BaseType base) {
        std::vector<int> dimensions;
        while (match(TokenKind::LBracket)) {
            int dimension = 0;
            if (!check(TokenKind::RBracket)) {
                const Token size =
                    expect(TokenKind::IntLiteral, "array size");
                dimension = size.intValue;
            }
            expect(TokenKind::RBracket, "']'");
            dimensions.push_back(dimension);
        }
        return Type{base, std::move(dimensions)};
    }

    std::unique_ptr<FuncDecl> parseFunction(const DeclSpec& spec,
                                            const Token& name) {
        expect(TokenKind::LParen, "'('");
        std::vector<std::unique_ptr<ParamDecl>> params;

        if (!check(TokenKind::RParen)) {
            if (check(TokenKind::KwVoid) &&
                peek(1).kind == TokenKind::RParen) {
                advance();
            } else {
                for (;;) {
                    const DeclSpec paramSpec = parseDeclSpec();
                    const Type paramType =
                        parseDeclaratorType(paramSpec.base);
                    if (paramType.isVoid()) {
                        fail(peek(), "function parameter cannot be void");
                    }
                    const Token paramName =
                        expect(TokenKind::Identifier, "parameter name");
                    params.push_back(std::make_unique<ParamDecl>(
                        paramName.text, paramType, paramName.line));
                    if (!match(TokenKind::Comma)) {
                        break;
                    }
                }
            }
        }
        expect(TokenKind::RParen, "')'");

        auto body = parseCompoundStmt();
        auto function = std::make_unique<FuncDecl>(name.line);
        function->name = name.text;
        function->returnType = Type{spec.base};
        function->params = std::move(params);
        function->body = std::move(body);
        return function;
    }

    void parseVariableDeclarators(
        const DeclSpec& spec, const Token& firstName,
        std::vector<std::unique_ptr<VarDecl>>& declarations) {
        Token name = firstName;

        for (;;) {
            if (spec.base == BaseType::VOID) {
                fail(name, "variable cannot have type void");
            }

            Type type = parseDeclaratorType(spec.base);
            std::unique_ptr<Expr> initializer;
            if (match(TokenKind::Assign)) {
                initializer = parseExpression();
            }

            auto declaration = std::make_unique<VarDecl>(name.line);
            declaration->name = name.text;
            declaration->type = std::move(type);
            declaration->isConst = spec.isConst;
            declaration->initializer = std::move(initializer);
            declarations.push_back(std::move(declaration));

            if (!match(TokenKind::Comma)) {
                break;
            }
            name = expect(TokenKind::Identifier, "identifier");
        }

        expect(TokenKind::Semicolon, "';'");
    }

    std::unique_ptr<CompoundStmt> parseCompoundStmt() {
        const Token open = expect(TokenKind::LBrace, "'{'");
        auto block = std::make_unique<CompoundStmt>(open.line);

        while (!check(TokenKind::RBrace) && !check(TokenKind::End)) {
            if (isDeclStart()) {
                const DeclSpec spec = parseDeclSpec();
                const Token name =
                    expect(TokenKind::Identifier, "identifier");
                if (check(TokenKind::LParen)) {
                    fail(peek(), "nested functions are not supported");
                }

                std::vector<std::unique_ptr<VarDecl>> declarations;
                parseVariableDeclarators(spec, name, declarations);
                for (auto& declaration : declarations) {
                    block->statements.push_back(std::move(declaration));
                }
            } else {
                block->statements.push_back(parseStatement());
            }
        }

        expect(TokenKind::RBrace, "'}'");
        return block;
    }

    std::unique_ptr<Stmt> parseStatement() {
        if (check(TokenKind::LBrace)) {
            return parseCompoundStmt();
        }

        if (match(TokenKind::Semicolon)) {
            const Token& semicolon = tokens_[index_ - 1];
            return std::make_unique<ExprStmt>(semicolon.line);
        }

        if (check(TokenKind::KwReturn)) {
            const Token keyword = advance();
            std::unique_ptr<Expr> value;
            if (!check(TokenKind::Semicolon)) {
                value = parseExpression();
            }
            expect(TokenKind::Semicolon, "';'");

            auto statement = std::make_unique<ReturnStmt>(keyword.line);
            statement->value = std::move(value);
            return statement;
        }

        if (check(TokenKind::KwIf)) {
            const Token keyword = advance();
            expect(TokenKind::LParen, "'('");
            auto condition = parseExpression();
            expect(TokenKind::RParen, "')'");
            auto thenBranch = parseStatement();

            std::unique_ptr<Stmt> elseBranch;
            if (match(TokenKind::KwElse)) {
                elseBranch = parseStatement();
            }

            auto statement = std::make_unique<IfStmt>(keyword.line);
            statement->condition = std::move(condition);
            statement->thenBranch = std::move(thenBranch);
            statement->elseBranch = std::move(elseBranch);
            return statement;
        }

        if (check(TokenKind::KwWhile)) {
            const Token keyword = advance();
            expect(TokenKind::LParen, "'('");
            auto condition = parseExpression();
            expect(TokenKind::RParen, "')'");
            auto body = parseStatement();

            auto statement = std::make_unique<WhileStmt>(keyword.line);
            statement->condition = std::move(condition);
            statement->body = std::move(body);
            return statement;
        }

        if (isDeclStart()) {
            fail(peek(), "declaration must be inside a block");
        }

        auto expression = parseExpression();
        const int line = expression->line;
        expect(TokenKind::Semicolon, "';'");

        auto statement = std::make_unique<ExprStmt>(line);
        statement->expr = std::move(expression);
        return statement;
    }

    std::unique_ptr<Expr> parseExpression() {
        return parseAssignment();
    }

    std::unique_ptr<Expr> parseAssignment() {
        auto expression = parseLogicalOr();
        if (check(TokenKind::Assign)) {
            const Token op = advance();
            auto assignment = std::make_unique<AssignExpr>(op.line);
            assignment->target = std::move(expression);
            assignment->value = parseAssignment();
            return assignment;
        }
        return expression;
    }

    std::unique_ptr<Expr> parseLogicalOr() {
        auto expression = parseLogicalAnd();
        while (check(TokenKind::OrOr)) {
            const Token op = advance();
            auto rhs = parseLogicalAnd();
            expression = makeBinary(BinaryOp::Or, op, std::move(expression),
                                    std::move(rhs));
        }
        return expression;
    }

    std::unique_ptr<Expr> parseLogicalAnd() {
        auto expression = parseEquality();
        while (check(TokenKind::AndAnd)) {
            const Token op = advance();
            auto rhs = parseEquality();
            expression = makeBinary(BinaryOp::And, op, std::move(expression),
                                    std::move(rhs));
        }
        return expression;
    }

    std::unique_ptr<Expr> parseEquality() {
        auto expression = parseRelational();
        for (;;) {
            BinaryOp op;
            if (check(TokenKind::Eq)) {
                op = BinaryOp::Eq;
            } else if (check(TokenKind::Ne)) {
                op = BinaryOp::Ne;
            } else {
                break;
            }
            const Token token = advance();
            auto rhs = parseRelational();
            expression = makeBinary(op, token, std::move(expression),
                                    std::move(rhs));
        }
        return expression;
    }

    std::unique_ptr<Expr> parseRelational() {
        auto expression = parseAdditive();
        for (;;) {
            BinaryOp op;
            if (check(TokenKind::Lt)) {
                op = BinaryOp::Lt;
            } else if (check(TokenKind::Gt)) {
                op = BinaryOp::Gt;
            } else if (check(TokenKind::Le)) {
                op = BinaryOp::Le;
            } else if (check(TokenKind::Ge)) {
                op = BinaryOp::Ge;
            } else {
                break;
            }
            const Token token = advance();
            auto rhs = parseAdditive();
            expression = makeBinary(op, token, std::move(expression),
                                    std::move(rhs));
        }
        return expression;
    }

    std::unique_ptr<Expr> parseAdditive() {
        auto expression = parseMultiplicative();
        for (;;) {
            BinaryOp op;
            if (check(TokenKind::Plus)) {
                op = BinaryOp::Add;
            } else if (check(TokenKind::Minus)) {
                op = BinaryOp::Sub;
            } else {
                break;
            }
            const Token token = advance();
            auto rhs = parseMultiplicative();
            expression = makeBinary(op, token, std::move(expression),
                                    std::move(rhs));
        }
        return expression;
    }

    std::unique_ptr<Expr> parseMultiplicative() {
        auto expression = parseUnary();
        for (;;) {
            BinaryOp op;
            if (check(TokenKind::Star)) {
                op = BinaryOp::Mul;
            } else if (check(TokenKind::Slash)) {
                op = BinaryOp::Div;
            } else if (check(TokenKind::Percent)) {
                op = BinaryOp::Mod;
            } else {
                break;
            }
            const Token token = advance();
            auto rhs = parseUnary();
            expression = makeBinary(op, token, std::move(expression),
                                    std::move(rhs));
        }
        return expression;
    }

    std::unique_ptr<Expr> parseUnary() {
        if (check(TokenKind::Plus) || check(TokenKind::Minus) ||
            check(TokenKind::Bang)) {
            const Token op = advance();
            auto operand = parseUnary();
            auto expression = std::make_unique<UnaryExpr>(op.line);
            if (op.kind == TokenKind::Plus) {
                expression->op = UnaryOp::Plus;
            } else if (op.kind == TokenKind::Minus) {
                expression->op = UnaryOp::Minus;
            } else {
                expression->op = UnaryOp::Not;
            }
            expression->operand = std::move(operand);
            return expression;
        }
        return parsePrimary();
    }

    std::unique_ptr<Expr> parsePrimary() {
        if (check(TokenKind::IntLiteral)) {
            const Token literal = advance();
            auto expression = std::make_unique<IntLiteral>(literal.line);
            expression->value = literal.intValue;
            return expression;
        }

        if (check(TokenKind::KwTrue) || check(TokenKind::KwFalse)) {
            const Token literal = advance();
            auto expression = std::make_unique<BoolLiteral>(literal.line);
            expression->value = literal.kind == TokenKind::KwTrue;
            return expression;
        }

        if (check(TokenKind::Identifier)) {
            const Token identifier = advance();
            if (match(TokenKind::LParen)) {
                auto call = std::make_unique<FuncCall>(identifier.line);
                call->callee = identifier.text;
                if (!check(TokenKind::RParen)) {
                    for (;;) {
                        call->args.push_back(parseExpression());
                        if (!match(TokenKind::Comma)) {
                            break;
                        }
                    }
                }
                expect(TokenKind::RParen, "')'");
                return call;
            }

            auto reference = std::make_unique<VarRef>(identifier.line);
            reference->name = identifier.text;
            return reference;
        }

        if (match(TokenKind::LParen)) {
            auto expression = parseExpression();
            expect(TokenKind::RParen, "')'");
            return expression;
        }

        fail(peek(), "expected an expression");
    }

    std::unique_ptr<Expr> makeBinary(BinaryOp op, const Token& token,
                                     std::unique_ptr<Expr> lhs,
                                     std::unique_ptr<Expr> rhs) {
        auto expression = std::make_unique<BinaryExpr>(token.line);
        expression->op = op;
        expression->lhs = std::move(lhs);
        expression->rhs = std::move(rhs);
        return expression;
    }

    std::vector<Token> tokens_;
    std::size_t index_ = 0;
};

}  // namespace

ParseResult parseSysY(const std::string& source) {
    ParseResult result;

    try {
        Lexer lexer(source);
        Parser parser(lexer.tokenize());
        result.program = parser.parseProgram();
    } catch (const ParseError& error) {
        result.error = error.message;
        result.errorLine = error.line;
    }

    return result;
}
