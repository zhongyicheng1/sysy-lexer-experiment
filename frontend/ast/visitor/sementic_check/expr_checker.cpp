#include <frontend/ast/visitor/sementic_check/ast_checker.h>
#include <debug.h>

namespace FE::AST
{
    namespace
    {
        size_t productOfDims(const std::vector<int>& dims, size_t start)
        {
            size_t result = 1;
            for (size_t i = start; i < dims.size(); ++i) result *= static_cast<size_t>(dims[i]);
            return result;
        }
    }  // namespace

    bool ASTChecker::checkAssignableType(Type* lhsType, Type* rhsType, const Node& node)
    {
        if (!lhsType || !rhsType)
        {
            reportError(node, "cannot determine assignment type");
            return false;
        }
        if (isVoidType(lhsType) || isVoidType(rhsType))
        {
            reportError(node, "void value cannot participate in assignment");
            return false;
        }

        if (isPointerType(lhsType))
        {
            if (!isPointerType(rhsType))
            {
                reportError(node,
                    "cannot assign value of type '" + rhsType->toString() + "' to pointer type '" +
                        lhsType->toString() + "'");
                return false;
            }

            if (pointerElementType(lhsType) != pointerElementType(rhsType))
            {
                reportError(node,
                    "cannot assign pointer of type '" + rhsType->toString() + "' to '" + lhsType->toString() + "'");
                return false;
            }
            return true;
        }

        if (isPointerType(rhsType))
        {
            reportError(node,
                "cannot assign pointer value of type '" + rhsType->toString() + "' to '" + lhsType->toString() + "'");
            return false;
        }

        if (!isNumericType(lhsType) || !isNumericType(rhsType))
        {
            reportError(node,
                "cannot assign value of type '" + rhsType->toString() + "' to '" + lhsType->toString() + "'");
            return false;
        }

        return true;
    }

    bool ASTChecker::visit(LeftValExpr& node)
    {
        node.isLval                = false;
        node.attr.val.isConstexpr  = false;
        node.attr.val.value.type   = voidType;

        if (!node.entry)
        {
            reportError(node, "left-value expression has no symbol");
            return false;
        }

        const std::string& name = node.entry->getName();
        VarAttr*           var  = findVariable(node.entry);
        if (!var)
        {
            if (findFunction(node.entry))
                reportError(node, "function '" + name + "' cannot be used as a variable");
            else
                reportError(node, "undefined variable '" + name + "'");
            return false;
        }

        bool ok       = true;
        bool hasIndex = node.indices && !node.indices->empty();
        size_t indexCount = 0;
        size_t providedIndexCount = node.indices ? node.indices->size() : 0;

        if (node.indices)
        {
            for (auto* index : *node.indices)
            {
                if (!index)
                {
                    reportError(node, "empty index expression for '" + name + "'");
                    ok = false;
                    continue;
                }

                if (!apply(*this, *index))
                {
                    ok = false;
                    continue;
                }

                Type* indexType = index->attr.val.value.type;
                if (isVoidType(indexType) || !isIntegerType(indexType))
                {
                    reportError(*index, "array index must be an integer");
                    ok = false;
                    continue;
                }
                indexCount++;
            }
        }

        Type* baseType  = var->type;
        bool  isPointer = isPointerType(baseType);
        bool  isArray   = !var->arrayDims.empty();

        Type* resultType = nullptr;
        bool  assignable = false;
        bool  constExpr  = false;
        VarValue constValue;

        if (!isPointer && !isArray)
        {
            if (hasIndex)
            {
                reportError(node, "scalar variable '" + name + "' cannot be indexed");
                ok = false;
            }
            else
            {
                resultType = baseType;
                assignable = true;
                if (var->isConstDecl && !var->initList.empty())
                {
                    constExpr  = true;
                    constValue = var->initList[0];
                }
            }
        }
        else if (isPointer)
        {
            // 指针形参：arrayDims 存放去掉第一维后的维度
            size_t rank = var->arrayDims.size();
            if (providedIndexCount > rank + 1)
            {
                reportError(node, "too many subscripts for '" + name + "'");
                ok = false;
            }
            if (!hasIndex)
            {
                resultType = baseType;  // PtrType
                assignable = true;
            }
            else if (indexCount >= rank + 1)
            {
                auto* ptrType = static_cast<PtrType*>(baseType);
                resultType    = ptrType->base;
                assignable    = true;
            }
            else
            {
                resultType = baseType;  // 部分下标后仍是指针
                assignable = false;
            }
        }
        else
        {
            // 普通数组：arrayDims 是完整维度
            size_t rank = var->arrayDims.size();
            if (providedIndexCount > rank)
            {
                reportError(node, "too many subscripts for '" + name + "'");
                ok = false;
            }
            if (!hasIndex)
            {
                resultType = TypeFactory::getPtrType(baseType);  // 数组退化为首元素指针
                assignable = false;
            }
            else if (indexCount >= rank)
            {
                resultType = baseType;
                assignable = true;

                if (var->isConstDecl && var->initList.size() == productOfDims(var->arrayDims, 0) &&
                    node.indices->size() >= rank)
                {
                    bool              allConst = true;
                    std::vector<int>  indices;
                    for (size_t i = 0; i < rank; ++i)
                    {
                        ExprNode* index = (*node.indices)[i];
                        if (!index || !index->attr.val.isConstexpr || !isIntegerType(index->attr.val.value.type))
                        {
                            allConst = false;
                            break;
                        }
                        indices.push_back(index->attr.val.getInt());
                    }

                    if (allConst)
                    {
                        size_t flat = 0;
                        bool   inRange = true;
                        for (size_t i = 0; i < rank; ++i)
                        {
                            int dim = var->arrayDims[i];
                            if (indices[i] < 0 || indices[i] >= dim)
                            {
                                inRange = false;
                                break;
                            }
                            flat = flat * static_cast<size_t>(dim) + static_cast<size_t>(indices[i]);
                        }
                        if (inRange && flat < var->initList.size())
                        {
                            constExpr  = true;
                            constValue = var->initList[flat];
                        }
                    }
                }
            }
            else
            {
                resultType = TypeFactory::getPtrType(baseType);
                assignable = false;
            }
        }

        node.isLval               = assignable;
        node.attr.val.value.type  = resultType ? resultType : voidType;
        node.attr.val.isConstexpr = constExpr;
        if (constExpr) node.attr.val.value = constValue;

        if (!resultType) ok = false;
        return ok;
    }

    bool ASTChecker::visit(LiteralExpr& node)
    {
        // 示例实现：字面量表达式的语义检查
        // 字面量总是编译期常量，直接设置属性
        node.attr.val.isConstexpr = true;
        node.attr.val.value       = node.literal;
        return true;
    }

    bool ASTChecker::visit(UnaryExpr& node)
    {
        if (!node.expr)
        {
            reportError(node, "unary expression has no operand");
            node.attr.val.value.type = voidType;
            return false;
        }

        bool ok = apply(*this, *node.expr);
        if (!ok)
        {
            node.attr.val.value.type  = voidType;
            node.attr.val.isConstexpr = false;
            return false;
        }

        Type* operandType = node.expr->attr.val.value.type;
        if (isVoidType(operandType) || !isScalarType(operandType))
        {
            reportError(node, "unary operator requires a non-void scalar operand");
            node.attr.val.value.type  = voidType;
            node.attr.val.isConstexpr = false;
            return false;
        }

        bool      hasError = false;
        ExprValue result   = typeInfer(node.expr->attr.val, node.op, node, hasError);
        node.attr.val      = result;

        if (hasError || isVoidType(result.value.type)) ok = false;
        return ok;
    }

    bool ASTChecker::visit(BinaryExpr& node)
    {
        if (!node.lhs || !node.rhs)
        {
            reportError(node, "binary expression has a missing operand");
            node.attr.val.value.type = voidType;
            return false;
        }

        bool ok = true;

        if (node.op == Operator::ASSIGN)
        {
            auto* lhs = dynamic_cast<LeftValExpr*>(node.lhs);
            if (!lhs)
            {
                reportError(*node.lhs, "left operand of assignment must be a left-value expression");
                node.attr.val.value.type = voidType;
                return false;
            }

            bool lhsOk = apply(*this, *node.lhs);
            bool rhsOk = apply(*this, *node.rhs);
            ok         = lhsOk && rhsOk && ok;

            if (!lhsOk || !rhsOk)
            {
                node.attr.val.value       = VarValue();
                node.attr.val.value.type  = lhs->attr.val.value.type ? lhs->attr.val.value.type : voidType;
                node.attr.val.isConstexpr = false;
                return false;
            }

            if (!lhs->isLval)
            {
                reportError(*node.lhs, "expression is not assignable");
                ok = false;
            }

            VarAttr* var = findVariable(lhs->entry);
            if (var)
            {
                if (var->isConstDecl)
                {
                    reportError(*node.lhs, "cannot assign to const variable '" + lhs->entry->getName() + "'");
                    ok = false;
                }
                if (!var->arrayDims.empty() && (!lhs->indices || lhs->indices->empty()))
                {
                    reportError(*node.lhs, "cannot assign to an array '" + lhs->entry->getName() + "'");
                    ok = false;
                }
            }

            Type* lhsType = lhs->attr.val.value.type;
            Type* rhsType = node.rhs->attr.val.value.type;
            if (!checkAssignableType(lhsType, rhsType, node)) ok = false;

            node.attr.val.value       = VarValue();
            node.attr.val.value.type  = lhsType ? lhsType : voidType;
            node.attr.val.isConstexpr = false;
            return ok;
        }

        bool lhsOk = apply(*this, *node.lhs);
        bool rhsOk = apply(*this, *node.rhs);
        ok         = lhsOk && rhsOk && ok;

        if (!lhsOk || !rhsOk)
        {
            node.attr.val.value.type  = voidType;
            node.attr.val.isConstexpr = false;
            return false;
        }

        Type* lhsType = node.lhs->attr.val.value.type;
        Type* rhsType = node.rhs->attr.val.value.type;

        if (isVoidType(lhsType) || isVoidType(rhsType))
        {
            reportError(node, "void value cannot participate in a binary operation");
            node.attr.val.value.type  = voidType;
            node.attr.val.isConstexpr = false;
            return false;
        }

        if (!isScalarType(lhsType) || !isScalarType(rhsType))
        {
            reportError(node, "binary operator requires scalar operands");
            node.attr.val.value.type  = voidType;
            node.attr.val.isConstexpr = false;
            return false;
        }

        // type_infer.cpp 对非常量操作数的浮点 % / 位运算会提前返回，
        // 这里在调用前补上类型检查。
        if ((node.op == Operator::MOD || node.op == Operator::BITOR || node.op == Operator::BITAND) &&
            (!isIntegerType(lhsType) || !isIntegerType(rhsType)))
        {
            bool bothConst = node.lhs->attr.val.isConstexpr && node.rhs->attr.val.isConstexpr;
            if (!bothConst)
            {
                reportError(node, "operator '" + toString(node.op) + "' requires integer operands");
                node.attr.val.value.type  = voidType;
                node.attr.val.isConstexpr = false;
                return false;
            }
        }

        bool      hasError = false;
        ExprValue result   = typeInfer(node.lhs->attr.val, node.rhs->attr.val, node.op, node, hasError);
        node.attr.val      = result;

        if (hasError || isVoidType(result.value.type)) ok = false;
        return ok;
    }

    bool ASTChecker::checkCallArgs(CallExpr& node, FuncDeclStmt& func)
    {
        size_t argCount   = node.args ? node.args->size() : 0;
        size_t paramCount = func.params ? func.params->size() : 0;
        bool   ok         = true;

        const std::string& name = node.func ? node.func->getName() : std::string("<unknown>");

        if (argCount != paramCount)
        {
            reportError(node,
                "function '" + name + "' expects " + std::to_string(paramCount) + " arguments, but " +
                    std::to_string(argCount) + " were provided");
            ok = false;
        }

        size_t common = argCount < paramCount ? argCount : paramCount;
        for (size_t i = 0; i < common; ++i)
        {
            ExprNode*        arg   = (*node.args)[i];
            ParamDeclarator* param = (*func.params)[i];
            if (!arg || !param) continue;

            if (!apply(*this, *arg))
            {
                ok = false;
                continue;
            }

            Type* actualType = arg->attr.val.value.type;
            Type* expectType = param->type;
            if (param->dims && !param->dims->empty()) expectType = TypeFactory::getPtrType(param->type);

            if (isVoidType(actualType) || isVoidType(expectType))
            {
                reportError(*arg, "void value cannot be passed as a function argument");
                ok = false;
                continue;
            }

            if (isPointerType(expectType))
            {
                if (!isPointerType(actualType))
                {
                    reportError(*arg,
                        "argument of type '" + actualType->toString() + "' is not compatible with pointer parameter '" +
                            expectType->toString() + "'");
                    ok = false;
                }
                else if (pointerElementType(expectType) != pointerElementType(actualType))
                {
                    reportError(*arg,
                        "argument of type '" + actualType->toString() + "' is not compatible with pointer parameter '" +
                            expectType->toString() + "'");
                    ok = false;
                }
            }
            else if (isScalarType(expectType))
            {
                if (!isScalarType(actualType))
                {
                    reportError(*arg,
                        "argument of type '" + actualType->toString() + "' is not compatible with parameter '" +
                            expectType->toString() + "'");
                    ok = false;
                }
            }
            else
            {
                reportError(*arg, "function parameter has an invalid type");
                ok = false;
            }
        }

        // 参数数量不匹配时，额外实参也要访问，以便发现其中的未定义变量等错误
        if (argCount > paramCount && node.args)
        {
            for (size_t i = paramCount; i < argCount; ++i)
            {
                if ((*node.args)[i] && !apply(*this, *(*node.args)[i])) ok = false;
            }
        }

        return ok;
    }

    bool ASTChecker::visit(CallExpr& node)
    {
        if (!node.func)
        {
            reportError(node, "function call has no function symbol");
            node.attr.val.value.type  = voidType;
            node.attr.val.isConstexpr = false;
            return false;
        }

        FuncDeclStmt* func = findFunction(node.func);
        if (!func)
        {
            if (findVariable(node.func))
                reportError(node, "'" + node.func->getName() + "' is not a function");
            else
                reportError(node, "undefined function '" + node.func->getName() + "'");
            node.attr.val.value.type  = voidType;
            node.attr.val.isConstexpr = false;
            return false;
        }

        // 局部变量可以遮蔽同名函数；此时调用应解析到变量而不是函数
        if (findVariable(node.func))
        {
            reportError(node, "'" + node.func->getName() + "' is not a function");
            node.attr.val.value.type  = voidType;
            node.attr.val.isConstexpr = false;
            return false;
        }

        bool ok = checkCallArgs(node, *func);

        node.attr.val.value.type  = func->retType ? func->retType : voidType;
        node.attr.val.isConstexpr = false;
        return ok;
    }

    bool ASTChecker::visit(CommaExpr& node)
    {
        if (!node.exprs || node.exprs->empty())
        {
            reportError(node, "empty comma expression");
            node.attr.val.value.type  = voidType;
            node.attr.val.isConstexpr = false;
            return false;
        }

        bool      ok   = true;
        ExprValue last;
        for (auto* expr : *node.exprs)
        {
            if (!expr) continue;
            if (!apply(*this, *expr)) ok = false;
            last = expr->attr.val;
        }

        node.attr.val = last;
        return ok;
    }
}  // namespace FE::AST
