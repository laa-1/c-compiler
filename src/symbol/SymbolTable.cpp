#include "SymbolTable.h"

#include <cassert>
#include "../ast/node/expression/IntegerLiteralExpression.h"
#include "ArraySymbol.h"
#include "FunctionSymbol.h"
#include "PointerSymbol.h"
#include "ScalarSymbol.h"

SymbolTable::SymbolTable(Scope *rootScope) : rootScope(rootScope) {}

SymbolTable::~SymbolTable() {
    delete rootScope;
}

int SymbolTable::getTypeMemoryUse(Type *type) {
    switch (type->getClass()) {
        case TypeClass::ARRAY_TYPE:
            return getTypeMemoryUse(((ArrayType *) type)->elemType) * ((ArrayType *) type)->size;
        case TypeClass::SCALAR_TYPE:
            switch (((ScalarType *) type)->baseType) {
                case BaseType::VOID:
                    assert(false);
                case BaseType::CHAR:
                case BaseType::UNSIGNED_CHAR:
                    return 1;
                case BaseType::SHORT:
                case BaseType::UNSIGNED_SHORT:
                    return 2;
                case BaseType::INT:
                case BaseType::LONG_INT:
                case BaseType::UNSIGNED_INT:
                case BaseType::UNSIGNED_LONG_INT:
                case BaseType::FLOAT:
                    return 4;
                case BaseType::LONG_LONG_INT:
                case BaseType::UNSIGNED_LONG_LONG_INT:
                case BaseType::DOUBLE:
                    return 8;
            }
        case TypeClass::FUNCTION_TYPE:
            return 0;
        case TypeClass::POINTER_TYPE:
            return 8;
    }
    assert(false);
}

void SymbolTable::calculateScopeAddress(Scope *scope, std::uint64_t &current) {
    std::uint64_t start = current;
    for (const auto &pair : scope->map) {
        Symbol *symbol = pair.second;
        switch (symbol->getClass()) {
            case SymbolClass::SCALAR_SYMBOL:
                ((ScalarSymbol *) symbol)->address = current;
                current += getTypeMemoryUse(((ScalarSymbol *) symbol)->type);
                break;
            case SymbolClass::POINTER_SYMBOL:
                ((PointerSymbol *) symbol)->address = current;
                current += getTypeMemoryUse(((PointerSymbol *) symbol)->type);
                break;
            case SymbolClass::ARRAY_SYMBOL:
                ((ArraySymbol *) symbol)->address = current;
                current += getTypeMemoryUse(((ArraySymbol *) symbol)->type);
                break;
            case SymbolClass::FUNCTION_SYMBOL:
            case SymbolClass::STATEMENT_SYMBOL:
                break;
        }
    }
    scope->memoryUseSelf = current - start;
}

void SymbolTable::calculateScopeAddressRecursively(Scope *scope, std::uint64_t &current) {
    std::uint64_t start = current;
    calculateScopeAddress(scope, current);
    scope->memoryUseRecursive = current - start;
    std::uint64_t childStartAddress = current;
    for (auto childScope : scope->childList) {
        current = childStartAddress; // 并列作用域的内存是可复用的
        calculateScopeAddressRecursively(childScope, current);
        if (current - start > scope->memoryUseRecursive) {
            scope->memoryUseRecursive = current - start;
        }
    }
}

SymbolTableIterator *SymbolTable::createIterator() {
    return new SymbolTableIterator(rootScope);
}

void SymbolTable::calculateAddress(std::uint64_t start) {
    startAddress = start;
    std::uint64_t currentAddress = start;
    calculateScopeAddress(rootScope, currentAddress);
    for (auto functionScope : rootScope->childList) {
        std::uint64_t functionStartAddress = 0;
        calculateScopeAddressRecursively(functionScope, functionStartAddress);
    }
    memoryUseRootScope = rootScope->memoryUseSelf;
}

bool SymbolTable::checkGlobal(const std::string &identifier) {
    return (*rootScope)[identifier] != nullptr;
}

std::map<std::uint64_t, std::uint64_t> SymbolTable::createFunctionMemoryUseMap() {
    std::map<std::uint64_t, std::uint64_t> memoryUseMap;
    for (auto functionScope : rootScope->childList) {
        memoryUseMap[((FunctionSymbol *) (*rootScope)[functionScope->name])->address] = functionScope->memoryUseRecursive;
    }
    return memoryUseMap;
}

std::uint64_t SymbolTable::getStartAddress() const {
    return startAddress;
}

std::uint64_t SymbolTable::getMemoryUseRootScope() const {
    return memoryUseRootScope;
}
