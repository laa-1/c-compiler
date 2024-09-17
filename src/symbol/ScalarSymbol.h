#pragma once

#include "Symbol.h"
#include "../ast/node/type/ArrayType.h"
#include "../ast/node/type/FunctionType.h"
#include "../ast/node/type/PointerType.h"
#include "../ast/node/type/ScalarType.h"

class ScalarSymbol : public Symbol {
public:
    ScalarType *type = nullptr;
    std::uint64_t address = 0;

    ScalarSymbol(const std::string &identifier, ScalarType *type) : Symbol(identifier), type(type) {}

    ~ScalarSymbol() override = default;

    SymbolClass getClass() override {
        return SymbolClass::SCALAR_SYMBOL;
    }
};
