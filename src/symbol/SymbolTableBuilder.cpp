#include "SymbolTableBuilder.h"

#include <utility>

#include "SymbolTable.h"
#include "Scope.h"

SymbolTableBuilder::SymbolTableBuilder() {
    rootScope = new Scope("", nullptr);
    scopeStack.push(rootScope);
}

SymbolTableBuilder::~SymbolTableBuilder() {
    delete rootScope; // 防止未调用build而内存泄露
}

Symbol *SymbolTableBuilder::operator[](const std::string &identifier) {
    return (*scopeStack.top())[identifier];
}

void SymbolTableBuilder::insertSymbol(Symbol *symbol) {
    scopeStack.top()->map[symbol->identifier] = symbol;
}

void SymbolTableBuilder::createScope(std::string name) {
    auto *child = new Scope(std::move(name), scopeStack.top());
    scopeStack.top()->childList.push_back(child);
    scopeStack.push(child);
}

void SymbolTableBuilder::exitScope() {
    scopeStack.pop();
}

SymbolTable *SymbolTableBuilder::build() {
    auto *symbolTable = new SymbolTable(rootScope);
    rootScope = nullptr; // 防止析构delete
    return symbolTable;
}
