#pragma once

#include <memory>
#include <map>
#include <string>
#include <cstdint>
#include <vector>
#include <stack>
#include "Symbol.h"

class Scope {
public:
    std::string name; // 作用域的名称，只有函数作用域才有名称，且和函数名标识符保持一致
    std::map<std::string, Symbol *> map;
    std::uint64_t memoryUseRecursive = 0; // 当前作用域及递归向下的所有子作用域的内存使用量
    std::uint64_t memoryUseSelf = 0; // 当前作用域的内存使用量
    Scope *parent = nullptr;
    std::vector<Scope *> childList;

public:
    Scope(std::string name, Scope *parent) : name(std::move(name)), parent(parent) {}
    ~Scope() {
        for (const auto &pair : map) {
            delete pair.second;
        }
        for (auto child : childList) {
            delete child;
        }
    }
    Symbol *operator[](const std::string &identifier) {
        // 递归向上查找该标识符
        Scope *upper = this;
        while (upper != nullptr) {
            if (upper->map.contains(identifier)) {
                return upper->map[identifier];
            }
            upper = upper->parent;
        }
        return nullptr;
    }
};
