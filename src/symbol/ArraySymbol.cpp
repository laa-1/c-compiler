#include "ArraySymbol.h"

ArraySymbol::ArraySymbol(const std::string &identifier, ArrayType *type) : Symbol(identifier), type(type) {}

SymbolClass ArraySymbol::getClass() {
    return SymbolClass::ARRAY_SYMBOL;
}

