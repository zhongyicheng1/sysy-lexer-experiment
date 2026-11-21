#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../AST.h"
#include "../../symbol/SymbolTable.h"

// Post-order semantic checker for SysY.  Diagnostics are accumulated instead
// of throwing, so one run reports as many independent errors as possible.
// Code generation must only run when check() returns true.
class SemanticCheck : public ASTVisitor {
public:
    SemanticCheck();

    SymbolTable symTable;
    std::vector<std::string> errors;
    bool hasMainFunc = false;

    bool check(ASTNode* root);

    const std::vector<std::string>& getErrors() const noexcept {
        return errors;
    }

    void reportError(int line, const std::string& category,
                     const std::string& msg);

    void visit(Program& node) override;
    void visit(FuncDecl& node) override;
    void visit(VarDecl& node) override;
    void visit(CompoundStmt& node) override;
    void visit(IfStmt& node) override;
    void visit(WhileStmt& node) override;
    void visit(ReturnStmt& node) override;
    void visit(ExprStmt& node) override;

    void visit(IntLiteral& node) override;
    void visit(BoolLiteral& node) override;
    void visit(VarRef& node) override;
    void visit(UnaryExpr& node) override;
    void visit(BinaryExpr& node) override;
    void visit(FuncCall& node) override;
    void visit(AssignExpr& node) override;

private:
    void reset();
    void registerBuiltins();
    void addBuiltin(const std::string& name, const Type& returnType,
                    std::vector<Type> paramTypes);

    void declareFunctions(Program& program);
    bool registerFunction(const FuncDecl& function);

    void checkCondition(const Expr* condition, int line,
                        const std::string& context);
    void checkMainSignature(const FuncDecl& function);

    static bool isNumeric(const Type& type);
    static bool isIntegral(const Type& type);
    static bool isAssignable(const Type& target, const Type& source);
    static std::string typeToString(const Type& type);

    static int evaluateUnary(UnaryOp op, int value);
    static int evaluateBinary(BinaryOp op, int lhs, int rhs);

    Type currentReturnType_;
    std::string currentFunctionName_;
    bool inFunction_ = false;
};
