#pragma once

#include <cstdint>
#include <string>

enum class SymbolClass {
    SCALAR_SYMBOL,
    POINTER_SYMBOL,
    ARRAY_SYMBOL,
    FUNCTION_SYMBOL,
    STATEMENT_SYMBOL,
};

/**
 * 所有符号类的抽象基类。
 * 用于在Scope类中实现多态。
 */
class Symbol {
public:
    std::string identifier; // 符号的标识符

    explicit Symbol(std::string identifier);
    virtual ~Symbol() = default;
    virtual SymbolClass getClass() = 0;
};
