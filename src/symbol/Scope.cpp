#include "Scope.h"

#include <utility>

Scope::Scope(std::string name, Scope *parent) : name(std::move(name)), parent(parent) {}

Scope::~Scope() {
    for (const auto &pair : map) {
        delete pair.second;
    }
    for (auto child : childList) {
        delete child;
    }
}

Symbol *Scope::operator[](const std::string &identifier) {
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

