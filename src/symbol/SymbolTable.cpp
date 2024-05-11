#include "SymbolTable.h"

#include <cassert>
#include "../ast/node/expression/IntegerLiteralExpression.h"
#include "ArraySymbol.h"
#include "FunctionSymbol.h"
#include "PointerSymbol.h"
#include "ScalarSymbol.h"

int getTypeMemoryUse(Type *type) {
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

void calculateScopeAddress(Scope *scope, std::uint64_t &currentAddress) {
    std::uint64_t startAddress = currentAddress;
    for (const auto &pair : scope->map) {
        Symbol *symbol = pair.second;
        switch (symbol->getClass()) {
            case SymbolClass::SCALAR_SYMBOL:
                ((ScalarSymbol *) symbol)->address = currentAddress;
                currentAddress += getTypeMemoryUse(((ScalarSymbol *) symbol)->type);
                break;
            case SymbolClass::POINTER_SYMBOL:
                ((PointerSymbol *) symbol)->address = currentAddress;
                currentAddress += getTypeMemoryUse(((PointerSymbol *) symbol)->type);
                break;
            case SymbolClass::ARRAY_SYMBOL:
                ((ArraySymbol *) symbol)->address = currentAddress;
                currentAddress += getTypeMemoryUse(((ArraySymbol *) symbol)->type);
                break;
            case SymbolClass::FUNCTION_SYMBOL:
            case SymbolClass::STATEMENT_SYMBOL:
                break;
        }
    }
    scope->memoryUseSelf = currentAddress - startAddress;
}

void calculateScopeAddressRecursively(Scope *scope, std::uint64_t &currentAddress) {
    std::uint64_t startAddress = currentAddress;
    calculateScopeAddress(scope, currentAddress);
    scope->memoryUseRecursive = currentAddress - startAddress;
    std::uint64_t childStartAddress = currentAddress;
    for (auto childScope : scope->childList) {
        currentAddress = childStartAddress; // 并列作用域的内存是可复用的
        calculateScopeAddressRecursively(childScope, currentAddress);
        if (currentAddress - startAddress > scope->memoryUseRecursive) {
            scope->memoryUseRecursive = currentAddress - startAddress;
        }
    }
}

SymbolTable::SymbolTable(Scope *rootScope) : rootScope(rootScope) {}

SymbolTable::~SymbolTable() {
    delete rootScope;
}

bool SymbolTable::checkGlobal(const std::string &identifier) {
    return (*rootScope)[identifier] != nullptr;
}

void SymbolTable::calculateAddress(std::uint64_t start) {
    startAddress = start;
    std::uint64_t currentAddress = start;
    calculateScopeAddress(rootScope, currentAddress);
    for (auto functionScope : rootScope->childList) {
        std::uint64_t functionStartAddress = 0;
        calculateScopeAddressRecursively(functionScope, functionStartAddress);
    }
    memoryUseRoot = rootScope->memoryUseSelf;
}

SymbolTableIterator *SymbolTable::createIterator() {
    return new SymbolTableIterator(rootScope);
}

std::map<std::uint64_t, std::uint64_t> SymbolTable::createMemoryUseMap() {
    std::map<std::uint64_t, std::uint64_t> memoryUseMap;
    for (auto functionScope : rootScope->childList) {
        memoryUseMap[((FunctionSymbol *) (*rootScope)[functionScope->name])->address] = functionScope->memoryUseRecursive;
    }
    return memoryUseMap;
}

std::uint64_t SymbolTable::getStartAddress() const {
    return startAddress;
}

std::uint64_t SymbolTable::getMemoryUseRoot() const {
    return memoryUseRoot;
}
