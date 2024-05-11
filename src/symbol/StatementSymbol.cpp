#include "StatementSymbol.h"

StatementSymbol::StatementSymbol(const std::string &identifier) : Symbol(identifier) {}

SymbolClass StatementSymbol::getClass() {
    return SymbolClass::STATEMENT_SYMBOL;
}

