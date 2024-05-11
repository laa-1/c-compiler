#include "FunctionSymbol.h"

FunctionSymbol::FunctionSymbol(const std::string &identifier, FunctionType *type) : Symbol(identifier), type(type) {}

SymbolClass FunctionSymbol::getClass() {
    return SymbolClass::FUNCTION_SYMBOL;
}