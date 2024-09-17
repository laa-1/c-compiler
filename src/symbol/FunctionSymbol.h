#pragma once

#include "Symbol.h"
#include "../ast/node/type/ArrayType.h"
#include "../ast/node/type/FunctionType.h"
#include "../ast/node/type/PointerType.h"
#include "../ast/node/type/ScalarType.h"

class FunctionSymbol : public Symbol {
public:
    FunctionType *type = nullptr;
    std::uint64_t address = 0;

    FunctionSymbol(const std::string &identifier, FunctionType *type) : Symbol(identifier), type(type) {}

    ~FunctionSymbol() override = default;

    SymbolClass getClass() override {
        return SymbolClass::FUNCTION_SYMBOL;
    }
};
