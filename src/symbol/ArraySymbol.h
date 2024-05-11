#pragma once

#include "Symbol.h"
#include "../ast/node/type/ArrayType.h"
#include "../ast/node/type/FunctionType.h"
#include "../ast/node/type/PointerType.h"
#include "../ast/node/type/ScalarType.h"

class ArraySymbol : public Symbol {
public:
    ArrayType *type = nullptr;
    std::uint64_t address = 0;

    ArraySymbol(const std::string &identifier, ArrayType *type);
    ~ArraySymbol() override = default;
    SymbolClass getClass() override;
};
