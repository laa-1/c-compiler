#include "ScalarSymbol.h"

ScalarSymbol::ScalarSymbol(const std::string &identifier, ScalarType *type) : Symbol(identifier), type(type) {}

SymbolClass ScalarSymbol::getClass() {
    return SymbolClass::SCALAR_SYMBOL;
}

