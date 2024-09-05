#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include "../symbol/SymbolTable.h"
#include "../constant/StringConstantPool.h"
#include "../instruction/InstructionSequence.h"

/**
 * 字节码类。
 * 包含了虚拟机运行的所需的所有信息。
 */
class Bytecode {
private:
    Bytecode() = default;
    std::map<std::uint64_t, std::uint64_t> functionMemoryUseMap;
    std::vector<std::uint8_t> codeArea;
    std::vector<std::uint8_t> dataArea;

public:
    void outputToBinaryFile(std::unique_ptr<std::ofstream> file);
    void outputToHumanReadableFile(std::unique_ptr<std::ofstream> file);
    const std::map<std::uint64_t, std::uint64_t> &getMemoryUseMap() const;
    const std::vector<std::uint8_t> &getCodeArea() const;
    const std::vector<std::uint8_t> &getDataArea() const;
    static Bytecode *build(SymbolTable *symbolTable, StringConstantPool *stringConstantPool, InstructionSequence *instructionSequence);
    static Bytecode *build(std::unique_ptr<std::ifstream> file);
};
