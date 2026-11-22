#include <frontend/symbol/symbol_table.h>
#include <debug.h>

namespace FE::Sym
{
    void SymTable::reset_impl()
    {
        scopes.clear();
        // 全局作用域始终存在，reset 后回到全局作用域
        scopes.emplace_back();
    }

    void SymTable::enterScope_impl() { scopes.emplace_back(); }

    void SymTable::exitScope_impl()
    {
        // 全局作用域不允许被退出
        ASSERT(scopes.size() > 1 && "Cannot exit the global scope");
        scopes.pop_back();
    }

    void SymTable::addSymbol_impl(Entry* entry, FE::AST::VarAttr& attr)
    {
        ASSERT(!scopes.empty() && "Symbol table has no scope");
        ASSERT(entry != nullptr && "Cannot add a null symbol");

        // 记录声明所在作用域，供语义检查器判断“同层重定义”
        attr.scopeLevel   = getScopeDepth_impl();
        scopes.back()[entry] = attr;
    }

    FE::AST::VarAttr* SymTable::getSymbol_impl(Entry* entry)
    {
        if (!entry) return nullptr;

        // 从最内层作用域向外查找，实现内层遮蔽
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
        {
            auto found = it->find(entry);
            if (found != it->end()) return &found->second;
        }
        return nullptr;
    }

    bool SymTable::isGlobalScope_impl() { return scopes.size() <= 1; }

    int SymTable::getScopeDepth_impl() { return scopes.empty() ? 0 : static_cast<int>(scopes.size()) - 1; }
}  // namespace FE::Sym
