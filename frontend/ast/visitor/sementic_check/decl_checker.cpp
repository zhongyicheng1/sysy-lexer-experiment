#include <frontend/ast/visitor/sementic_check/ast_checker.h>
#include <debug.h>

namespace FE::AST
{
    namespace
    {
        VarValue zeroValueOf(Type* type)
        {
            if (type && type->getBaseType() == Type_t::FLOAT) return VarValue(0.0f);
            return VarValue(0);
        }

        size_t productOfDims(const std::vector<int>& dims, size_t start)
        {
            size_t result = 1;
            for (size_t i = start; i < dims.size(); ++i) result *= static_cast<size_t>(dims[i]);
            return result;
        }
    }  // namespace

    bool ASTChecker::visit(Initializer& node)
    {
        // 示例实现：单个初始化器的语义检查
        // 1) 访问初始化值表达式
        // 2) 将子表达式的属性拷贝到当前节点
        ASSERT(node.init_val && "Null initializer value");
        bool res  = apply(*this, *node.init_val);
        node.attr = node.init_val->attr;
        return res;
    }

    bool ASTChecker::visit(InitializerList& node)
    {
        // 示例实现：初始化器列表的语义检查
        // 遍历列表中的每个初始化器并逐个访问
        if (!node.init_list) return true;
        bool res = true;
        for (auto* init : *(node.init_list))
        {
            if (!init) continue;
            res = apply(*this, *init) && res;
        }
        return res;
    }

    bool ASTChecker::visit(VarDeclarator& node)
    {
        // 该节点本身不携带声明类型/作用域信息，真正的变量注册由 VarDeclaration 完成。
        // 这里只保证节点可被单独访问时，左值标记与初始化表达式得到检查。
        if (!node.lval)
        {
            reportError(node, "variable declarator has no lvalue");
            return false;
        }

        auto* lval = dynamic_cast<LeftValExpr*>(node.lval);
        if (!lval)
        {
            reportError(*node.lval, "variable declarator must contain a left-value expression");
            return false;
        }

        lval->isLval = true;
        lval->attr.val.isConstexpr = false;

        if (!node.init) return true;

        bool res = apply(*this, *node.init);
        node.attr = node.init->attr;
        return res;
    }

    bool ASTChecker::visit(ParamDeclarator& node)
    {
        if (!node.entry)
        {
            reportError(node, "parameter has no symbol entry");
            return false;
        }
        if (!node.type || isVoidType(node.type))
        {
            reportError(node, "parameter '" + node.entry->getName() + "' cannot have void type");
            return false;
        }

        bool     ok   = true;
        VarAttr  attr(node.type, false, 0);
        Type*    exprType = node.type;

        if (node.dims && !node.dims->empty())
        {
            std::vector<int> tailDims;

            for (size_t i = 0; i < node.dims->size(); ++i)
            {
                ExprNode* dim = (*node.dims)[i];
                if (!dim)
                {
                    reportError(node, "parameter '" + node.entry->getName() + "' has an empty array dimension");
                    ok = false;
                    continue;
                }

                if (!apply(*this, *dim))
                {
                    ok = false;
                    continue;
                }

                if (!dim->attr.val.isConstexpr || !isIntegerType(dim->attr.val.value.type))
                {
                    reportError(*dim, "array parameter dimension must be a constant integer expression");
                    ok = false;
                    continue;
                }

                int value = dim->attr.val.getInt();
                if (i == 0)
                {
                    // 第一维在形参中会退化为指针；-1 表示 int a[]，正数表示 int a[N]。
                    if (value == 0 || value < -1)
                    {
                        reportError(*dim, "invalid array parameter dimension");
                        ok = false;
                    }
                    continue;
                }

                if (value <= 0)
                {
                    reportError(*dim, "array parameter dimension must be positive");
                    ok = false;
                    continue;
                }
                tailDims.push_back(value);
            }

            // 数组形参退化为指向元素的指针
            attr.type      = TypeFactory::getPtrType(node.type);
            attr.arrayDims = tailDims;
            exprType       = attr.type;
        }

        node.attr.val.value.type = exprType;
        node.attr.val.isConstexpr = false;

        if (!addVariable(node.entry, attr, node)) ok = false;
        return ok;
    }

    bool ASTChecker::visit(VarDeclaration& node)
    {
        if (!node.type)
        {
            reportError(node, "variable declaration has no type");
            return false;
        }
        if (isVoidType(node.type))
        {
            reportError(node, "variable type cannot be void");
            return false;
        }
        if (!node.decls) return true;

        bool isGlobal = symTable.isGlobalScope();
        bool ok       = true;

        for (auto* declarator : *node.decls)
        {
            if (!declarator) continue;

            auto* lval = dynamic_cast<LeftValExpr*>(declarator->lval);
            if (!lval || !lval->entry)
            {
                reportError(*declarator, "variable declarator has no valid left-value");
                ok = false;
                continue;
            }

            const std::string& name = lval->entry->getName();

            VarAttr attr(node.type, node.isConstDecl, 0);
            bool    dimsOk = true;

            // lval->indices 在声明中表示数组维度，而不是运行期下标
            if (lval->indices)
            {
                for (auto* dim : *lval->indices)
                {
                    if (!dim)
                    {
                        reportError(*declarator, "array dimension of '" + name + "' is empty");
                        dimsOk = false;
                        continue;
                    }

                    if (!apply(*this, *dim))
                    {
                        dimsOk = false;
                        continue;
                    }

                    if (!dim->attr.val.isConstexpr || !isIntegerType(dim->attr.val.value.type))
                    {
                        reportError(*dim, "array dimension of '" + name + "' must be a constant integer expression");
                        dimsOk = false;
                        continue;
                    }

                    int value = dim->attr.val.getInt();
                    if (value <= 0)
                    {
                        reportError(*dim, "array dimension of '" + name + "' must be positive");
                        dimsOk = false;
                        continue;
                    }
                    attr.arrayDims.push_back(value);
                }
            }

            // 声明左值本身也应被标记，供后续阶段使用
            lval->isLval                = true;
            lval->attr.val.isConstexpr  = false;
            lval->attr.val.value.type   = attr.arrayDims.empty() ? node.type : TypeFactory::getPtrType(node.type);

            if (!dimsOk) ok = false;

            if (!addVariable(lval->entry, attr, *declarator))
            {
                ok = false;
                continue;
            }

            if (node.isConstDecl && !declarator->init)
            {
                reportError(*declarator, "const variable '" + name + "' must be initialized");
                ok = false;
            }

            bool requireConst = isGlobal || node.isConstDecl;
            bool fillInitList = isGlobal || node.isConstDecl;

            size_t total = 1;
            for (int dim : attr.arrayDims) total *= static_cast<size_t>(dim);

            std::vector<VarValue> initList(total, zeroValueOf(node.type));
            bool                  initValid = true;

            if (declarator->init)
            {
                if (attr.arrayDims.empty())
                {
                    checkScalarInitializer(
                        declarator->init, node.type, 0, requireConst, isGlobal, initList, initValid);
                }
                else
                {
                    if (dynamic_cast<InitializerList*>(declarator->init))
                    {
                        checkInitializer(declarator->init, node.type, attr.arrayDims, 0, 0, requireConst, isGlobal,
                            initList, initValid);
                    }
                    else
                    {
                        reportError(*declarator->init, "array '" + name + "' must be initialized with a brace list");
                        initValid = false;
                    }
                }
            }

            if (!initValid) ok = false;

            // VarAttr 在 addVariable 时已被复制进符号表，这里把初值信息补写回去
            VarAttr* stored = symTable.getSymbol(lval->entry);
            if (stored)
            {
                if (fillInitList) stored->initList = initList;
                if (isGlobal) glbSymbols[lval->entry] = *stored;
            }
        }

        return ok;
    }

    bool ASTChecker::checkScalarInitializer(FE::AST::InitDecl* init, Type* baseType, size_t index,
        bool requireConst, bool isGlobal, std::vector<VarValue>& initList, bool& ok)
    {
        if (!init) return ok;

        if (index >= initList.size())
        {
            reportError(*init, "too many initializers");
            ok = false;
            return false;
        }

        if (auto* list = dynamic_cast<InitializerList*>(init))
        {
            if (!list->init_list || list->init_list->empty()) return ok;

            // 标量/单个元素外多余的花括号：取第一个标量叶子
            for (auto* child : *list->init_list)
            {
                if (!child) continue;
                return checkScalarInitializer(child, baseType, index, requireConst, isGlobal, initList, ok);
            }
            return ok;
        }

        auto* single = dynamic_cast<Initializer*>(init);
        if (!single)
        {
            reportError(*init, "invalid initializer");
            ok = false;
            return false;
        }
        if (!single->init_val) return ok;

        if (!apply(*this, *single->init_val))
        {
            ok = false;
            return false;
        }

        Type* actualType = single->init_val->attr.val.value.type;
        if (isVoidType(actualType) || !isScalarType(actualType))
        {
            reportError(*single->init_val, "initializer must be a scalar value");
            ok = false;
            return false;
        }
        if (!isNumericType(baseType))
        {
            reportError(*single->init_val, "cannot initialize a non-numeric variable with this expression");
            ok = false;
            return false;
        }

        if (requireConst && !single->init_val->attr.val.isConstexpr)
        {
            reportError(*single->init_val,
                isGlobal ? "global initializer must be a constant expression"
                         : "const initializer must be a constant expression");
            ok = false;
        }

        const VarValue& value = single->init_val->attr.val.value;
        if (baseType->getBaseType() == Type_t::FLOAT)
            initList[index] = VarValue(value.getFloat());
        else
            initList[index] = VarValue(value.getInt());

        return ok;
    }

    bool ASTChecker::checkInitializer(InitDecl* init, Type* baseType, const std::vector<int>& dims, size_t depth,
        size_t baseIndex, bool requireConst, bool isGlobal, std::vector<VarValue>& initList, bool& ok)
    {
        if (!init) return ok;
        if (!baseType)
        {
            reportError(*init, "initializer has no element type");
            ok = false;
            return false;
        }

        if (dims.empty() || depth >= dims.size())
            return checkScalarInitializer(init, baseType, baseIndex, requireConst, isGlobal, initList, ok);

        size_t elementSize = productOfDims(dims, depth + 1);
        size_t end         = baseIndex + static_cast<size_t>(dims[depth]) * elementSize;
        if (end > initList.size())
        {
            reportError(*init, "initializer exceeds array size");
            ok = false;
            end = initList.size();
        }

        if (auto* list = dynamic_cast<InitializerList*>(init))
        {
            if (!list->init_list) return ok;

            size_t cursor = baseIndex;
            for (auto* child : *list->init_list)
            {
                if (!child) continue;
                if (cursor >= end)
                {
                    reportError(*list, "too many initializers");
                    ok = false;
                    break;
                }

                if (elementSize > 1 && dynamic_cast<InitializerList*>(child))
                {
                    size_t element = (cursor - baseIndex) / elementSize;
                    if (element >= static_cast<size_t>(dims[depth]))
                    {
                        reportError(*child, "too many initializers");
                        ok = false;
                        break;
                    }

                    size_t childBase = baseIndex + element * elementSize;
                    checkInitializer(
                        child, baseType, dims, depth + 1, childBase, requireConst, isGlobal, initList, ok);
                    cursor = childBase + elementSize;
                }
                else
                {
                    // 标量叶子（或多余花括号）按顺序填入当前聚合体的下一个标量槽位
                    checkScalarInitializer(child, baseType, cursor, requireConst, isGlobal, initList, ok);
                    cursor++;
                }
            }
            return ok;
        }

        if (auto* single = dynamic_cast<Initializer*>(init))
        {
            if (depth == 0)
            {
                reportError(*single, "array initializer must be brace-enclosed");
                ok = false;
                return false;
            }

            // 深层聚合体使用单个标量时，只初始化第一个元素
            return checkScalarInitializer(init, baseType, baseIndex, requireConst, isGlobal, initList, ok);
        }

        reportError(*init, "invalid initializer");
        ok = false;
        return false;
    }
}  // namespace FE::AST
