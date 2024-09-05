#pragma once

#include "../symbol/SymbolTable.h"
#include "../constant/StringConstantPool.h"

class AddressCalculator {
private:
    SymbolTable *symbolTable = nullptr;
    StringConstantPool *stringConstantPool = nullptr;

private:
    AddressCalculator(SymbolTable *symbolTable, StringConstantPool *stringConstantPool);
    void calculate();

public:
    static void calculate(SymbolTable *symbolTable, StringConstantPool *stringConstantPool);
};
