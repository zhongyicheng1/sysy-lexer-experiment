#include "AST.h"

void IntLiteral::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void BoolLiteral::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void VarRef::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void UnaryExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void BinaryExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void FuncCall::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void AssignExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void ExprStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void ReturnStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void CompoundStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void IfStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void WhileStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void VarDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void FuncDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void Program::accept(ASTVisitor& visitor) { visitor.visit(*this); }
