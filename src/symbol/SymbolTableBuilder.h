#pragma once

#include <memory>
#include <stack>
#include "SymbolTable.h"

class SymbolTableBuilder {
private:
    Scope *rootScope;
    std::stack<Scope *> scopeStack;

public:
    SymbolTableBuilder();
    ~SymbolTableBuilder();
    Symbol *operator[](const std::string &identifier);
    void insertSymbol(Symbol *symbol);
    void createScope(std::string name);
    void exitScope();
    SymbolTable *build();
};
