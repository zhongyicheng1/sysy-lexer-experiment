#include <frontend/ast/visitor/sementic_check/ast_checker.h>
#include <debug.h>

namespace FE::AST
{
    bool ASTChecker::checkCondition(ExprNode& expr)
    {
        if (!apply(*this, expr)) return false;

        Type* type = expr.attr.val.value.type;
        if (isVoidType(type) || !isScalarType(type))
        {
            reportError(expr, "condition must be a non-void scalar expression");
            return false;
        }
        return true;
    }

    bool ASTChecker::checkReturnType(Type* actual, Type* expected, const Node& node)
    {
        if (!expected)
        {
            reportError(node, "function has no return type");
            return false;
        }
        if (isVoidType(expected))
        {
            reportError(node, "void function cannot return a value");
            return false;
        }
        if (!actual || isVoidType(actual))
        {
            reportError(node, "return value has void type");
            return false;
        }

        if (isTensorType(expected))
        {
            if (!isTensorType(actual))
            {
                reportError(node, "return value is not a tensor");
                return false;
            }
            if (tensorElementType(actual)->getBaseType() != tensorElementType(expected)->getBaseType())
            {
                reportError(node, "tensor element type does not match the function return type");
                return false;
            }
            return true;
        }

        if (isPointerType(expected))
        {
            if (!isPointerType(actual) || pointerElementType(actual) != pointerElementType(expected))
            {
                reportError(node,
                    "return type '" + actual->toString() + "' is not compatible with '" + expected->toString() + "'");
                return false;
            }
            return true;
        }

        if (isPointerType(actual))
        {
            reportError(node,
                "return type '" + actual->toString() + "' is not compatible with '" + expected->toString() + "'");
            return false;
        }

        if (!isNumericType(expected) || !isNumericType(actual))
        {
            reportError(node,
                "return type '" + actual->toString() + "' is not compatible with '" + expected->toString() + "'");
            return false;
        }

        return true;
    }

    bool ASTChecker::visit(ExprStmt& node)
    {
        // 示例实现：表达式语句的语义检查
        // 空表达式直接通过，否则访问内部表达式
        if (!node.expr) return true;
        return apply(*this, *node.expr);
    }

    bool ASTChecker::visit(FuncDeclStmt& node)
    {
        if (!node.entry)
        {
            reportError(node, "function has no symbol entry");
            return false;
        }

        if (!symTable.isGlobalScope())
        {
            reportError(node, "function definition is not allowed here");
            return false;
        }

        const std::string& name = node.entry->getName();

        auto registered = funcDecls.find(node.entry);
        if (registered == funcDecls.end() || registered->second != &node)
        {
            reportError(node, "function '" + name + "' was not registered (duplicate declaration?)");
            return false;
        }

        if (!node.retType)
        {
            reportError(node, "function '" + name + "' has no return type");
            return false;
        }

        // 保存外层上下文（正常情况下函数不会嵌套，但保证状态可恢复）
        FuncDeclStmt* savedFunc      = curFunc;
        bool          savedInFunc    = inFunction;
        Type*         savedRetType   = curFuncRetType;
        bool          savedHasReturn = funcHasReturn;
        size_t        savedLoopDepth = loopDepth;

        curFunc        = &node;
        inFunction     = true;
        curFuncRetType = node.retType;
        funcHasReturn  = false;
        loopDepth      = 0;

        symTable.enterScope();

        bool ok = true;
        if (node.params)
        {
            for (auto* param : *node.params)
            {
                if (!param) continue;
                ok = apply(*this, *param) && ok;
            }
        }

        if (node.body)
        {
            // 函数形参与函数体最外层声明属于同一作用域；嵌套块仍由
            // BlockStmt::visit 进入新作用域，从而支持内层遮蔽。
            if (auto* bodyBlock = dynamic_cast<BlockStmt*>(node.body))
            {
                if (bodyBlock->stmts)
                {
                    for (auto* stmt : *bodyBlock->stmts)
                    {
                        if (!stmt) continue;
                        ok = apply(*this, *stmt) && ok;
                    }
                }
            }
            else
            {
                ok = apply(*this, *node.body) && ok;
            }
        }

        if (!isVoidType(node.retType) && !funcHasReturn && name != "main")
        {
            reportError(node, "non-void function '" + name + "' must return a value");
            ok = false;
        }

        symTable.exitScope();

        curFunc        = savedFunc;
        inFunction     = savedInFunc;
        curFuncRetType = savedRetType;
        funcHasReturn  = savedHasReturn;
        loopDepth      = savedLoopDepth;

        return ok;
    }

    bool ASTChecker::visit(VarDeclStmt& node)
    {
        if (!node.decl) return true;
        return apply(*this, *node.decl);
    }

    bool ASTChecker::visit(BlockStmt& node)
    {
        symTable.enterScope();

        bool ok = true;
        if (node.stmts)
        {
            for (auto* stmt : *node.stmts)
            {
                if (!stmt) continue;
                ok = apply(*this, *stmt) && ok;
            }
        }

        symTable.exitScope();
        return ok;
    }

    bool ASTChecker::visit(ReturnStmt& node)
    {
        if (!inFunction)
        {
            reportError(node, "return statement outside a function");
            return false;
        }

        funcHasReturn = true;

        Type* expected = curFuncRetType ? curFuncRetType : voidType;

        if (node.retExpr)
        {
            bool ok = apply(*this, *node.retExpr);
            if (!ok) return false;

            if (isVoidType(expected))
            {
                reportError(node, "void function cannot return a value");
                return false;
            }

            Type* actual = node.retExpr->attr.val.value.type;
            if (!checkReturnType(actual, expected, node)) ok = false;
            return ok;
        }

        if (!isVoidType(expected))
        {
            reportError(node, "non-void function must return a value");
            return false;
        }

        return true;
    }

    bool ASTChecker::visit(WhileStmt& node)
    {
        bool ok = true;

        if (!node.cond)
        {
            reportError(node, "while statement has no condition");
            ok = false;
        }
        else if (!checkCondition(*node.cond))
        {
            ok = false;
        }

        loopDepth++;
        if (node.body) ok = apply(*this, *node.body) && ok;
        loopDepth--;

        return ok;
    }

    bool ASTChecker::visit(IfStmt& node)
    {
        bool ok = true;

        if (!node.cond)
        {
            reportError(node, "if statement has no condition");
            ok = false;
        }
        else if (!checkCondition(*node.cond))
        {
            ok = false;
        }

        if (node.thenStmt) ok = apply(*this, *node.thenStmt) && ok;
        if (node.elseStmt) ok = apply(*this, *node.elseStmt) && ok;

        return ok;
    }

    bool ASTChecker::visit(BreakStmt& node)
    {
        if (loopDepth == 0)
        {
            reportError(node, "break statement outside a loop");
            return false;
        }
        return true;
    }

    bool ASTChecker::visit(ContinueStmt& node)
    {
        if (loopDepth == 0)
        {
            reportError(node, "continue statement outside a loop");
            return false;
        }
        return true;
    }

    bool ASTChecker::visit(ForStmt& node)
    {
        symTable.enterScope();

        bool ok = true;

        if (node.init) ok = apply(*this, *node.init) && ok;

        if (node.cond && !checkCondition(*node.cond)) ok = false;

        loopDepth++;
        if (node.body) ok = apply(*this, *node.body) && ok;
        loopDepth--;

        if (node.step) ok = apply(*this, *node.step) && ok;

        symTable.exitScope();
        return ok;
    }
}  // namespace FE::AST
