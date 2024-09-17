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

class Symbol {
public:
    std::string identifier;

    explicit Symbol(std::string identifier) : identifier(std::move(identifier)) {}
    virtual ~Symbol() = default;
    virtual SymbolClass getClass() = 0;
};
