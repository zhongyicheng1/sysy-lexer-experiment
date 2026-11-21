#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../../interfaces/frontend/Type.h"

// Common information shared by every symbol table entry.
class SymbolEntry {
public:
    SymbolEntry() = default;

    SymbolEntry(std::string entryName, Type entryType, int entryScopeLevel = 0)
        : name(std::move(entryName)),
          type(std::move(entryType)),
          scopeLevel(entryScopeLevel) {}

    // Entries may be owned by the AST after their scope has been popped.
    virtual ~SymbolEntry() = default;

    std::string name;
    Type type;
    int scopeLevel = 0;
};

// A variable or constant declared in a function or at global scope.
class VarEntry : public SymbolEntry {
public:
    VarEntry() = default;

    VarEntry(std::string entryName, Type entryType, bool constant,
             int initialValue = 0, int entryScopeLevel = 0)
        : SymbolEntry(std::move(entryName), std::move(entryType),
                      entryScopeLevel),
          isConst(constant),
          initValue(initialValue) {}

    bool isConst = false;
    int initValue = 0;
};

// A function declaration or definition.
class FuncEntry : public SymbolEntry {
public:
    FuncEntry() = default;

    FuncEntry(std::string entryName, Type functionReturnType,
              std::vector<Type> parameterTypes = {}, int entryScopeLevel = 0)
        : SymbolEntry(std::move(entryName), functionReturnType,
                      entryScopeLevel),
          returnType(std::move(functionReturnType)),
          paramTypes(std::move(parameterTypes)) {}

    Type returnType;
    std::vector<Type> paramTypes;
};

// Lexically scoped symbol table.  The constructor installs the global scope;
// each enterScope()/exitScope() pair adds and removes one nested scope.
class SymbolTable {
public:
    SymbolTable();

    void enterScope();

    // Pops only the innermost scope.  The entries themselves are released
    // through their shared_ptr owners (often the AST), which remain valid.
    // The global scope is never popped.
    void exitScope();

    // Adds to the current scope only.  Returns false for a null entry or a
    // redeclaration in the current scope; a name in a parent scope may be
    // shadowed.
    bool addSymbol(std::shared_ptr<SymbolEntry> entry);

    // Looks up the name from the innermost scope outwards.
    std::shared_ptr<SymbolEntry> getSymbol(const std::string& name);
    std::shared_ptr<SymbolEntry> getSymbol(const std::string& name) const;

    std::size_t getScopeDepth() const noexcept { return scopes_.size(); }
    int getCurrentScopeLevel() const noexcept;

private:
    using Scope =
        std::unordered_map<std::string, std::shared_ptr<SymbolEntry>>;

    std::vector<Scope> scopes_;
};
