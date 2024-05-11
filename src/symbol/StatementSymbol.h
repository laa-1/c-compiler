#pragma once

#include "Symbol.h"

class StatementSymbol : public Symbol {
public:
    std::uint64_t address = 0;

    explicit StatementSymbol(const std::string &identifier);
    ~StatementSymbol() override = default;
    SymbolClass getClass() override;
};
