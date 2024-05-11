#include "BuiltinFunctionLibrary.h"

#include "../symbol/ScalarSymbol.h"
#include "../symbol/PointerSymbol.h"


void BuiltinFunctionLibrary::insertSymbol(std::unique_ptr<SymbolTableBuilder> &symbolTableBuilder) {
    std::vector<FunctionDeclaration *> functionDeclarationList;
    symbolTableBuilder->insertSymbol(new FunctionSymbol("scan_i64", new FunctionType(new ScalarType(BaseType::VOID, {}), {new PointerType(new ScalarType(BaseType::LONG_LONG_INT, {}), {})})));
    symbolTableBuilder->createScope("scan_i64");
    symbolTableBuilder->insertSymbol(new PointerSymbol("address", new PointerType(new ScalarType(BaseType::LONG_LONG_INT, {}), {})));
    symbolTableBuilder->exitScope();
    symbolTableBuilder->insertSymbol(new FunctionSymbol("scan_u64", new FunctionType(new ScalarType(BaseType::VOID, {}), {new PointerType(new ScalarType(BaseType::UNSIGNED_LONG_LONG_INT, {}), {})})));
    symbolTableBuilder->createScope("scan_u64");
    symbolTableBuilder->insertSymbol(new PointerSymbol("address", new PointerType(new ScalarType(BaseType::UNSIGNED_LONG_LONG_INT, {}), {})));
    symbolTableBuilder->exitScope();
    symbolTableBuilder->insertSymbol(new FunctionSymbol("scan_f64", new FunctionType(new ScalarType(BaseType::VOID, {}), {new PointerType(new ScalarType(BaseType::DOUBLE, {}), {})})));
    symbolTableBuilder->createScope("scan_f64");
    symbolTableBuilder->insertSymbol(new PointerSymbol("address", new PointerType(new ScalarType(BaseType::DOUBLE, {}), {})));
    symbolTableBuilder->exitScope();
    symbolTableBuilder->insertSymbol(new FunctionSymbol("scan_s", new FunctionType(new ScalarType(BaseType::VOID, {}), {new PointerType(new ScalarType(BaseType::CHAR, {}), {})})));
    symbolTableBuilder->createScope("scan_s");
    symbolTableBuilder->insertSymbol(new PointerSymbol("address", new PointerType(new ScalarType(BaseType::CHAR, {}), {})));
    symbolTableBuilder->exitScope();
    symbolTableBuilder->insertSymbol(new FunctionSymbol("print_i64", new FunctionType(new ScalarType(BaseType::VOID, {}), {new ScalarType(BaseType::LONG_LONG_INT, {})})));
    symbolTableBuilder->createScope("print_i64");
    symbolTableBuilder->insertSymbol(new ScalarSymbol("value", new ScalarType(BaseType::LONG_LONG_INT, {})));
    symbolTableBuilder->exitScope();
    symbolTableBuilder->insertSymbol(new FunctionSymbol("print_u64", new FunctionType(new ScalarType(BaseType::VOID, {}), {new ScalarType(BaseType::UNSIGNED_LONG_LONG_INT, {})})));
    symbolTableBuilder->createScope("print_u64");
    symbolTableBuilder->insertSymbol(new ScalarSymbol("value", new ScalarType(BaseType::UNSIGNED_LONG_LONG_INT, {})));
    symbolTableBuilder->exitScope();
    symbolTableBuilder->insertSymbol(new FunctionSymbol("print_f64", new FunctionType(new ScalarType(BaseType::VOID, {}), {new ScalarType(BaseType::DOUBLE, {})})));
    symbolTableBuilder->createScope("print_f64");
    symbolTableBuilder->insertSymbol(new ScalarSymbol("value", new ScalarType(BaseType::DOUBLE, {})));
    symbolTableBuilder->exitScope();
    symbolTableBuilder->insertSymbol(new FunctionSymbol("print_s", new FunctionType(new ScalarType(BaseType::VOID, {}), {new PointerType(new ScalarType(BaseType::CHAR, {}), {})})));
    symbolTableBuilder->createScope("print_s");
    symbolTableBuilder->insertSymbol(new PointerSymbol("address", new PointerType(new ScalarType(BaseType::CHAR, {}), {})));
    symbolTableBuilder->exitScope();
}

void BuiltinFunctionLibrary::generateCode(std::unique_ptr<SymbolTableIterator> &symbolTableIterator, std::unique_ptr<InstructionSequenceBuilder> &instructionSequenceBuilder) {
    ((FunctionSymbol *) (*symbolTableIterator)["scan_i64"])->address = instructionSequenceBuilder->getNextInstructionAddress();
    symbolTableIterator->switchScope();
    instructionSequenceBuilder->appendIn(BinaryDataType::I64);
    instructionSequenceBuilder->appendRet();
    symbolTableIterator->switchScope();
    ((FunctionSymbol *) (*symbolTableIterator)["scan_u64"])->address = instructionSequenceBuilder->getNextInstructionAddress();
    symbolTableIterator->switchScope();
    instructionSequenceBuilder->appendIn(BinaryDataType::U64);
    instructionSequenceBuilder->appendRet();
    symbolTableIterator->switchScope();
    ((FunctionSymbol *) (*symbolTableIterator)["scan_f64"])->address = instructionSequenceBuilder->getNextInstructionAddress();
    symbolTableIterator->switchScope();
    instructionSequenceBuilder->appendIn(BinaryDataType::F64);
    instructionSequenceBuilder->appendRet();
    symbolTableIterator->switchScope();
    ((FunctionSymbol *) (*symbolTableIterator)["scan_s"])->address = instructionSequenceBuilder->getNextInstructionAddress();
    symbolTableIterator->switchScope();
    instructionSequenceBuilder->appendIn();
    instructionSequenceBuilder->appendRet();
    symbolTableIterator->switchScope();
    ((FunctionSymbol *) (*symbolTableIterator)["print_i64"])->address = instructionSequenceBuilder->getNextInstructionAddress();
    symbolTableIterator->switchScope();
    instructionSequenceBuilder->appendOut(BinaryDataType::I64);
    instructionSequenceBuilder->appendRet();
    symbolTableIterator->switchScope();
    ((FunctionSymbol *) (*symbolTableIterator)["print_u64"])->address = instructionSequenceBuilder->getNextInstructionAddress();
    symbolTableIterator->switchScope();
    instructionSequenceBuilder->appendOut(BinaryDataType::U64);
    instructionSequenceBuilder->appendRet();
    symbolTableIterator->switchScope();
    ((FunctionSymbol *) (*symbolTableIterator)["print_f64"])->address = instructionSequenceBuilder->getNextInstructionAddress();
    symbolTableIterator->switchScope();
    instructionSequenceBuilder->appendOut(BinaryDataType::F64);
    instructionSequenceBuilder->appendRet();
    symbolTableIterator->switchScope();
    ((FunctionSymbol *) (*symbolTableIterator)["print_s"])->address = instructionSequenceBuilder->getNextInstructionAddress();
    symbolTableIterator->switchScope();
    instructionSequenceBuilder->appendOut();
    instructionSequenceBuilder->appendRet();
    symbolTableIterator->switchScope();
}


