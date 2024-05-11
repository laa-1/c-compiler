#pragma once

#include <memory>
#include <stack>
#include "SymbolTable.h"

/**
 * 用于构建SymbolTable的Builder类。
 * 封装了作用域的嵌套问题。
 */
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
