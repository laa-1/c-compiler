#pragma once

#include <memory>
#include <map>
#include <string>
#include <cstdint>
#include <vector>
#include <stack>
#include "Symbol.h"

/**
 * 作用域类。
 * 多个作用域会组成一个多叉树结构。
 */
class Scope {
public:
    std::string name; // 作用域的名称，只有函数作用域才有名称，且和函数名标识符保持一致
    std::map<std::string, Symbol *> map;
    std::uint64_t memoryUseRecursive = 0; // 当前作用域及递归向下的所有子作用域的内存使用量
    std::uint64_t memoryUseSelf = 0; // 当前作用域的内存使用量
    Scope *parent = nullptr;
    std::vector<Scope *> childList;

public:
    Scope(std::string name, Scope *parent);
    ~Scope();
    // 从当前作用域及递归向上的所有上层作用域中查找符号
    Symbol *operator[](const std::string &identifier);
};
