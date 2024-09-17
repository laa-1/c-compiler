#pragma onc

#include <vector>
#include <memory>
#include "../symbol/SymbolTableBuilder.h"
#include "../symbol/SymbolTable.h"
#include "../instruction/InstructionSequenceBuilder.h"

/**
 * 用于辅助插入内建函数的类。
 */
class BuiltInFunctionInserter{
public:
    static void insertSymbol(std::unique_ptr<SymbolTableBuilder> &symbolTableBuilder);
    static void insertCode(std::unique_ptr<SymbolTableIterator> &symbolTableIterator, std::unique_ptr<InstructionSequenceBuilder> &instructionSequenceBuilder);
};
