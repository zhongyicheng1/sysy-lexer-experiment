#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../../interfaces/frontend/NodeAttr.h"

// Minimal SysY AST used by the semantic checker.  Nodes own their children
// through unique_ptr and expose a conventional visitor interface so the
// checker does not depend on a parser-specific traversal style.
class ASTVisitor;

class Program;
class FuncDecl;
class VarDecl;
class ParamDecl;
class CompoundStmt;
class IfStmt;
class WhileStmt;
class ReturnStmt;
class ExprStmt;

class Expr;
class IntLiteral;
class BoolLiteral;
class VarRef;
class UnaryExpr;
class BinaryExpr;
class FuncCall;
class AssignExpr;

class ASTNode {
public:
    explicit ASTNode(int lineNumber = 0) : line(lineNumber) {}
    virtual ~ASTNode() = default;

    virtual void accept(ASTVisitor& visitor) = 0;

    int line = 0;
};

class Expr : public ASTNode {
public:
    using ASTNode::ASTNode;

    NodeAttr attr;
};

class Stmt : public ASTNode {
public:
    using ASTNode::ASTNode;
};

enum class UnaryOp {
    Plus,
    Minus,
    Not,
};

enum class BinaryOp {
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Lt,
    Gt,
    Le,
    Ge,
    Eq,
    Ne,
    And,
    Or,
};

inline const char* unaryOpSymbol(UnaryOp op) noexcept {
    switch (op) {
        case UnaryOp::Plus:
            return "+";
        case UnaryOp::Minus:
            return "-";
        case UnaryOp::Not:
            return "!";
    }
    return "?";
}

inline const char* binaryOpSymbol(BinaryOp op) noexcept {
    switch (op) {
        case BinaryOp::Add:
            return "+";
        case BinaryOp::Sub:
            return "-";
        case BinaryOp::Mul:
            return "*";
        case BinaryOp::Div:
            return "/";
        case BinaryOp::Mod:
            return "%";
        case BinaryOp::Lt:
            return "<";
        case BinaryOp::Gt:
            return ">";
        case BinaryOp::Le:
            return "<=";
        case BinaryOp::Ge:
            return ">=";
        case BinaryOp::Eq:
            return "==";
        case BinaryOp::Ne:
            return "!=";
        case BinaryOp::And:
            return "&&";
        case BinaryOp::Or:
            return "||";
    }
    return "?";
}

class IntLiteral : public Expr {
public:
    using Expr::Expr;

    void accept(ASTVisitor& visitor) override;

    int value = 0;
};

class BoolLiteral : public Expr {
public:
    using Expr::Expr;

    void accept(ASTVisitor& visitor) override;

    bool value = false;
};

class VarRef : public Expr {
public:
    using Expr::Expr;

    void accept(ASTVisitor& visitor) override;

    std::string name;
};

class UnaryExpr : public Expr {
public:
    using Expr::Expr;

    void accept(ASTVisitor& visitor) override;

    UnaryOp op = UnaryOp::Plus;
    std::unique_ptr<Expr> operand;
};

class BinaryExpr : public Expr {
public:
    using Expr::Expr;

    void accept(ASTVisitor& visitor) override;

    BinaryOp op = BinaryOp::Add;
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;
};

class FuncCall : public Expr {
public:
    using Expr::Expr;

    void accept(ASTVisitor& visitor) override;

    std::string callee;
    std::vector<std::unique_ptr<Expr>> args;
};

class AssignExpr : public Expr {
public:
    using Expr::Expr;

    void accept(ASTVisitor& visitor) override;

    std::unique_ptr<Expr> target;
    std::unique_ptr<Expr> value;
};

class ExprStmt : public Stmt {
public:
    using Stmt::Stmt;

    void accept(ASTVisitor& visitor) override;

    std::unique_ptr<Expr> expr;
};

class ReturnStmt : public Stmt {
public:
    using Stmt::Stmt;

    void accept(ASTVisitor& visitor) override;

    std::unique_ptr<Expr> value;
};

class CompoundStmt : public Stmt {
public:
    using Stmt::Stmt;

    void accept(ASTVisitor& visitor) override;

    std::vector<std::unique_ptr<Stmt>> statements;
};

class IfStmt : public Stmt {
public:
    using Stmt::Stmt;

    void accept(ASTVisitor& visitor) override;

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;
};

class WhileStmt : public Stmt {
public:
    using Stmt::Stmt;

    void accept(ASTVisitor& visitor) override;

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
};

struct ParamDecl {
    ParamDecl() = default;

    ParamDecl(std::string paramName, Type paramType, int lineNumber = 0)
        : name(std::move(paramName)),
          type(std::move(paramType)),
          line(lineNumber) {}

    std::string name;
    Type type;
    int line = 0;
};

// Declarations are statements in SysY blocks and may also appear at global
// scope, where Program stores them separately.
class VarDecl : public Stmt {
public:
    using Stmt::Stmt;

    void accept(ASTVisitor& visitor) override;

    std::string name;
    Type type;
    bool isConst = false;
    std::unique_ptr<Expr> initializer;
};

class FuncDecl : public ASTNode {
public:
    using ASTNode::ASTNode;

    void accept(ASTVisitor& visitor) override;

    std::string name;
    Type returnType;
    std::vector<std::unique_ptr<ParamDecl>> params;
    std::unique_ptr<CompoundStmt> body;
};

class Program : public ASTNode {
public:
    using ASTNode::ASTNode;

    void accept(ASTVisitor& visitor) override;

    std::vector<std::unique_ptr<VarDecl>> globals;
    std::vector<std::unique_ptr<FuncDecl>> functions;
};

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;

    virtual void visit(Program& node) = 0;
    virtual void visit(FuncDecl& node) = 0;
    virtual void visit(VarDecl& node) = 0;
    virtual void visit(CompoundStmt& node) = 0;
    virtual void visit(IfStmt& node) = 0;
    virtual void visit(WhileStmt& node) = 0;
    virtual void visit(ReturnStmt& node) = 0;
    virtual void visit(ExprStmt& node) = 0;

    virtual void visit(IntLiteral& node) = 0;
    virtual void visit(BoolLiteral& node) = 0;
    virtual void visit(VarRef& node) = 0;
    virtual void visit(UnaryExpr& node) = 0;
    virtual void visit(BinaryExpr& node) = 0;
    virtual void visit(FuncCall& node) = 0;
    virtual void visit(AssignExpr& node) = 0;
};
