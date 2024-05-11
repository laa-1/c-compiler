#pragma once

#include <memory>
#include <map>
#include <string>
#include <cstdint>
#include <vector>
#include <stack>
#include "Scope.h"

/**
 * 用于访问符号表的辅助类。
 * 封装了作用域的访问顺序问题。
 * 不完全是对符号表进行迭代（因为存在重复访问同一作用域）。
 */
class SymbolTableIterator {
private:
    struct VisitStatus {
        Scope *scope = nullptr;
        int nextChildIndex = 0;
    };
    std::stack<VisitStatus> visitStatusStack;

public:
    explicit SymbolTableIterator(Scope *rootScope);
    Symbol *operator[](const std::string &identifier);
    // 切换作用域，包含进入一个新的子作用域和退出当前作用域回到上层作用域这两种情况（因为作用域存在先访问上层，然后访问下层，接着再次访问上层的情况）
    void switchScope();
};
