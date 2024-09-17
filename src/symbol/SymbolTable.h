#pragma once

#include "Scope.h"
#include "SymbolTableIterator.h"
#include "FunctionSymbol.h"

/**
 * 符号表类。
 */
class SymbolTable {
private:
    Scope *rootScope = nullptr; // 根作用域
    std::uint64_t startAddress = 0;
    std::uint64_t memoryUseRootScope = 0; // 根作用域的内存使用量（字符串常量池不包含在内）

private:
    int getTypeMemoryUse(Type *type);
    void calculateScopeAddress(Scope *scope, std::uint64_t &current);
    void calculateScopeAddressRecursively(Scope *scope, std::uint64_t &current);

public:
    explicit SymbolTable(Scope *rootScope);
    ~SymbolTable();
    SymbolTableIterator *createIterator();
    void calculateAddress(std::uint64_t start);
    bool checkGlobal(const std::string &identifier);
    std::map<std::uint64_t, std::uint64_t> createFunctionMemoryUseMap();
    [[nodiscard]] std::uint64_t getStartAddress() const;
    [[nodiscard]] std::uint64_t getMemoryUseRootScope() const;
};
