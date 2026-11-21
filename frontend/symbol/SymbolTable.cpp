#include "SymbolTable.h"

SymbolTable::SymbolTable() {
    // Global scope.  Every SymbolTable always has at least one scope.
    scopes_.emplace_back();
}

void SymbolTable::enterScope() {
    scopes_.emplace_back();
}

void SymbolTable::exitScope() {
    if (scopes_.size() > 1) {
        // Removing the map only releases the table's shared_ptr references.
        // AST nodes that also own an entry keep it alive.
        scopes_.pop_back();
    }
}

bool SymbolTable::addSymbol(std::shared_ptr<SymbolEntry> entry) {
    if (!entry) {
        return false;
    }

    Scope& currentScope = scopes_.back();
    if (currentScope.find(entry->name) != currentScope.end()) {
        return false;
    }

    entry->scopeLevel = getCurrentScopeLevel();
    currentScope.emplace(entry->name, std::move(entry));
    return true;
}

std::shared_ptr<SymbolEntry> SymbolTable::getSymbol(const std::string& name) {
    return static_cast<const SymbolTable&>(*this).getSymbol(name);
}

std::shared_ptr<SymbolEntry> SymbolTable::getSymbol(
    const std::string& name) const {
    for (auto scope = scopes_.rbegin(); scope != scopes_.rend(); ++scope) {
        const auto found = scope->find(name);
        if (found != scope->end()) {
            return found->second;
        }
    }
    return nullptr;
}

int SymbolTable::getCurrentScopeLevel() const noexcept {
    return static_cast<int>(scopes_.size()) - 1;
}
