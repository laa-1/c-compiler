#include "PointerSymbol.h"

PointerSymbol::PointerSymbol(const std::string &identifier, PointerType *type) : Symbol(identifier), type(type) {}

SymbolClass PointerSymbol::getClass() {
    return SymbolClass::POINTER_SYMBOL;
}