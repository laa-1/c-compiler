#include "AddressCalculator.h"

AddressCalculator::AddressCalculator(SymbolTable *symbolTable, StringConstantPool *stringConstantPool) : symbolTable(symbolTable), stringConstantPool(stringConstantPool) {}

void AddressCalculator::calculate() {
    std::uint64_t startAddress = 8;
    stringConstantPool->calculateAddress(startAddress);
    startAddress += stringConstantPool->getMemoryUse();
    symbolTable->calculateAddress(startAddress);
}

void AddressCalculator::calculate(SymbolTable *symbolTable, StringConstantPool *stringConstantPool) {
    auto *addressCalculator = new AddressCalculator(symbolTable, stringConstantPool);
    addressCalculator->calculate();
    delete addressCalculator;
}