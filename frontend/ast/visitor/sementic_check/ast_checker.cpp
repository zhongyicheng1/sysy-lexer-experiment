#include <frontend/ast/visitor/sementic_check/ast_checker.h>
#include <debug.h>

namespace FE::AST
{
    bool ASTChecker::visit(Root& node)
    {
        // 每次检查根节点都从干净的全局作用域开始
        symTable.reset();
        glbSymbols.clear();
        errors.clear();

        mainExists     = false;
        funcHasReturn  = false;
        curFuncRetType = voidType;
        loopDepth      = 0;
        inFunction     = false;
        curFunc        = nullptr;

        std::vector<StmtNode*>* stmts = node.getStmts();
        if (!stmts || stmts->empty())
        {
            reportError(node, "empty program");
            return false;
        }

        bool ok = true;

        // 第一遍：预注册所有函数签名，支持前向调用和递归调用
        for (auto* stmt : *stmts)
        {
            if (!stmt) continue;

            auto* func = dynamic_cast<FuncDeclStmt*>(stmt);
            if (!func) continue;

            if (!func->entry)
            {
                reportError(*func, "function has no symbol entry");
                ok = false;
                continue;
            }

            const std::string& name = func->entry->getName();
            if (name == "main") mainExists = true;

            auto existing = funcDecls.find(func->entry);
            if (existing != funcDecls.end() && existing->second != func)
            {
                reportError(*func, "redefinition of function '" + name + "'");
                ok = false;
                continue;
            }

            funcDecls[func->entry] = func;
        }

        // 第二遍：按源码顺序检查顶层声明。全局作用域只允许变量声明和函数定义。
        for (auto* stmt : *stmts)
        {
            if (!stmt) continue;

            if (auto* varDecl = dynamic_cast<VarDeclStmt*>(stmt))
            {
                ok = apply(*this, *varDecl) && ok;
                continue;
            }

            if (auto* func = dynamic_cast<FuncDeclStmt*>(stmt))
            {
                auto existing = funcDecls.find(func->entry);
                if (existing != funcDecls.end() && existing->second == func)
                    ok = apply(*this, *func) && ok;
                continue;
            }

            reportError(*stmt, "ordinary statements are not allowed at global scope");
            ok = false;
        }

        // main 必须存在，且必须是 int main()
        if (!mainExists)
        {
            reportError(1, "main function is not defined");
            ok = false;
        }
        else
        {
            auto mainIt = funcDecls.find(Entry::getEntry("main"));
            if (mainIt != funcDecls.end() && mainIt->second)
            {
                FuncDeclStmt* mainFunc = mainIt->second;
                if (!mainFunc->retType || mainFunc->retType->getBaseType() != Type_t::INT ||
                    mainFunc->retType->getTypeGroup() != TypeGroup::BASIC)
                {
                    reportError(*mainFunc, "main function must return int");
                    ok = false;
                }
                if (mainFunc->params && !mainFunc->params->empty())
                {
                    reportError(*mainFunc, "main function must not have parameters");
                    ok = false;
                }
            }
        }

        return ok && errors.empty();
    }

    void ASTChecker::reportError(const Node& node, const std::string& message)
    {
        reportError(node.line_num, message);
    }

    void ASTChecker::reportError(int line, const std::string& message)
    {
        if (line < 0) line = 1;
        errors.push_back("line " + std::to_string(line) + ": " + message);
    }

    bool ASTChecker::addVariable(Entry* entry, VarAttr& attr, const Node& node)
    {
        if (!entry)
        {
            reportError(node, "null variable symbol");
            return false;
        }

        const std::string& name = entry->getName();

        if (symTable.isGlobalScope() && findFunction(entry))
        {
            reportError(node, "'" + name + "' is already declared as a function");
            return false;
        }

        VarAttr* old = symTable.getSymbol(entry);
        if (old && old->scopeLevel == symTable.getScopeDepth())
        {
            reportError(node, "redefinition of '" + name + "'");
            return false;
        }

        symTable.addSymbol(entry, attr);

        // 全局变量同时保留在 glbSymbols 中，供成员二 codegen 使用
        if (symTable.isGlobalScope()) glbSymbols[entry] = attr;

        return true;
    }

    VarAttr* ASTChecker::findVariable(Entry* entry)
    {
        if (!entry) return nullptr;

        VarAttr* symbol = symTable.getSymbol(entry);
        if (symbol) return symbol;

        auto it = glbSymbols.find(entry);
        if (it != glbSymbols.end()) return &it->second;

        return nullptr;
    }

    FuncDeclStmt* ASTChecker::findFunction(Entry* entry)
    {
        if (!entry) return nullptr;

        auto it = funcDecls.find(entry);
        return it == funcDecls.end() ? nullptr : it->second;
    }

    bool ASTChecker::isVoidType(Type* type) { return type && type->getBaseType() == Type_t::VOID; }

    bool ASTChecker::isScalarType(Type* type)
    {
        if (!type || type->getTypeGroup() != TypeGroup::BASIC) return false;
        Type_t base = type->getBaseType();
        return base == Type_t::BOOL || base == Type_t::INT || base == Type_t::LL || base == Type_t::FLOAT;
    }

    bool ASTChecker::isIntegerType(Type* type)
    {
        if (!type || type->getTypeGroup() != TypeGroup::BASIC) return false;
        Type_t base = type->getBaseType();
        return base == Type_t::BOOL || base == Type_t::INT || base == Type_t::LL;
    }

    bool ASTChecker::isNumericType(Type* type)
    {
        if (!type || type->getTypeGroup() != TypeGroup::BASIC) return false;
        Type_t base = type->getBaseType();
        return base == Type_t::BOOL || base == Type_t::INT || base == Type_t::LL || base == Type_t::FLOAT;
    }

    bool ASTChecker::isPointerType(Type* type)
    {
        return type && type->getTypeGroup() == TypeGroup::POINTER;
    }

    Type* ASTChecker::pointerElementType(Type* type)
    {
        if (!isPointerType(type)) return nullptr;
        return static_cast<PtrType*>(type)->base;
    }

    void ASTChecker::libFuncRegister()
    {
        // 示例实现：注册 SysY 标准库函数到 funcDecls 中
        // 这样在语义检查时可以识别并检查对库函数的调用
        // 包括：getint, getch, getarray, getfloat, getfarray,
        //      putint, putch, putarray, putfloat, putfarray,
        //      _sysy_starttime, _sysy_stoptime
        using SymEnt = FE::Sym::Entry;

        // int getint(), getch(), getarray(int a[]);
        static SymEnt* getint   = SymEnt::getEntry("getint");
        static SymEnt* getch    = SymEnt::getEntry("getch");
        static SymEnt* getarray = SymEnt::getEntry("getarray");

        // float getfloat();
        static SymEnt* getfloat = SymEnt::getEntry("getfloat");

        // int getfarray(float a[]);
        static SymEnt* getfarray = SymEnt::getEntry("getfarray");

        // void putint(int a), putch(int a), putarray(int n, int a[]);
        static SymEnt* putint   = SymEnt::getEntry("putint");
        static SymEnt* putch    = SymEnt::getEntry("putch");
        static SymEnt* putarray = SymEnt::getEntry("putarray");

        // void putfloat(float a);
        static SymEnt* putfloat = SymEnt::getEntry("putfloat");

        // void putfarray(int n, float a[]);
        static SymEnt* putfarray = SymEnt::getEntry("putfarray");

        // void starttime(), stoptime();
        static SymEnt* _sysy_starttime = SymEnt::getEntry("_sysy_starttime");
        static SymEnt* _sysy_stoptime  = SymEnt::getEntry("_sysy_stoptime");

        // int getint()
        funcDecls[getint] = new FuncDeclStmt(intType, getint, nullptr);

        // int getch()
        funcDecls[getch] = new FuncDeclStmt(intType, getch, nullptr);

        // int getarray(int a[])
        auto getarray_params = new std::vector<ParamDeclarator*>();
        auto getarray_param  = new ParamDeclarator(TypeFactory::getPtrType(intType), SymEnt::getEntry("a"));
        getarray_param->attr.val.value.type = TypeFactory::getPtrType(intType);
        getarray_params->push_back(getarray_param);
        funcDecls[getarray] = new FuncDeclStmt(intType, getarray, getarray_params);

        // float getfloat()
        funcDecls[getfloat] = new FuncDeclStmt(floatType, getfloat, nullptr);

        // int getfarray(float a[])
        auto getfarray_params = new std::vector<ParamDeclarator*>();
        auto getfarray_param  = new ParamDeclarator(TypeFactory::getPtrType(floatType), SymEnt::getEntry("a"));
        getfarray_param->attr.val.value.type = TypeFactory::getPtrType(floatType);
        getfarray_params->push_back(getfarray_param);
        funcDecls[getfarray] = new FuncDeclStmt(intType, getfarray, getfarray_params);

        // void putint(int a)
        auto putint_params                = new std::vector<ParamDeclarator*>();
        auto putint_param                 = new ParamDeclarator(intType, SymEnt::getEntry("a"));
        putint_param->attr.val.value.type = intType;
        putint_params->push_back(putint_param);
        funcDecls[putint] = new FuncDeclStmt(voidType, putint, putint_params);

        // void putch(int a)
        auto putch_params                = new std::vector<ParamDeclarator*>();
        auto putch_param                 = new ParamDeclarator(intType, SymEnt::getEntry("a"));
        putch_param->attr.val.value.type = intType;
        putch_params->push_back(putch_param);
        funcDecls[putch] = new FuncDeclStmt(voidType, putch, putch_params);

        // void putarray(int n, int a[])
        auto putarray_params                 = new std::vector<ParamDeclarator*>();
        auto putarray_param1                 = new ParamDeclarator(intType, SymEnt::getEntry("n"));
        putarray_param1->attr.val.value.type = intType;
        auto putarray_param2 = new ParamDeclarator(TypeFactory::getPtrType(intType), SymEnt::getEntry("a"));
        putarray_param2->attr.val.value.type = TypeFactory::getPtrType(intType);
        putarray_params->push_back(putarray_param1);
        putarray_params->push_back(putarray_param2);
        funcDecls[putarray] = new FuncDeclStmt(voidType, putarray, putarray_params);

        // void putfloat(float a)
        auto putfloat_params                = new std::vector<ParamDeclarator*>();
        auto putfloat_param                 = new ParamDeclarator(floatType, SymEnt::getEntry("a"));
        putfloat_param->attr.val.value.type = floatType;
        putfloat_params->push_back(putfloat_param);
        funcDecls[putfloat] = new FuncDeclStmt(voidType, putfloat, putfloat_params);

        // void putfarray(int n, float a[])
        auto putfarray_params                 = new std::vector<ParamDeclarator*>();
        auto putfarray_param1                 = new ParamDeclarator(intType, SymEnt::getEntry("n"));
        putfarray_param1->attr.val.value.type = intType;
        auto putfarray_param2 = new ParamDeclarator(TypeFactory::getPtrType(floatType), SymEnt::getEntry("a"));
        putfarray_param2->attr.val.value.type = TypeFactory::getPtrType(floatType);
        putfarray_params->push_back(putfarray_param1);
        putfarray_params->push_back(putfarray_param2);
        funcDecls[putfarray] = new FuncDeclStmt(voidType, putfarray, putfarray_params);

        // void _sysy_starttime(int lineno)
        auto starttime_params                = new std::vector<ParamDeclarator*>();
        auto starttime_param                 = new ParamDeclarator(intType, SymEnt::getEntry("lineno"));
        starttime_param->attr.val.value.type = intType;
        starttime_params->push_back(starttime_param);
        funcDecls[_sysy_starttime] = new FuncDeclStmt(voidType, _sysy_starttime, starttime_params);

        // void _sysy_stoptime(int lineno)
        auto stoptime_params                = new std::vector<ParamDeclarator*>();
        auto stoptime_param                 = new ParamDeclarator(intType, SymEnt::getEntry("lineno"));
        stoptime_param->attr.val.value.type = intType;
        stoptime_params->push_back(stoptime_param);
        funcDecls[_sysy_stoptime] = new FuncDeclStmt(voidType, _sysy_stoptime, stoptime_params);
    }
}  // namespace FE::AST
