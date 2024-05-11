#pragma once

#include "Symbol.h"
#include "../ast/node/type/ArrayType.h"
#include "../ast/node/type/FunctionType.h"
#include "../ast/node/type/PointerType.h"
#include "../ast/node/type/ScalarType.h"

class PointerSymbol : public Symbol {
public:
    PointerType *type = nullptr;
    std::uint64_t address = 0;

    PointerSymbol(const std::string &identifier, PointerType *type);
    ~PointerSymbol() override = default;
    SymbolClass getClass() override;
};
