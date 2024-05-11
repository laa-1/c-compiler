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
    std::uint64_t memoryUseRoot = 0; // 根作用域的内存使用量（字符串常量池不包含在内）

public:
    explicit SymbolTable(Scope *rootScope);
    ~SymbolTable();
    bool checkGlobal(const std::string &identifier);
    void calculateAddress(std::uint64_t start);
    SymbolTableIterator *createIterator();
    // 创建每个函数的入口地址到内存使用的映射表
    std::map<std::uint64_t, std::uint64_t> createMemoryUseMap();
    std::uint64_t getStartAddress() const;
    std::uint64_t getMemoryUseRoot() const;
};
