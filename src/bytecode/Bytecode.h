#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include "../symbol/SymbolTable.h"
#include "../constant/StringConstantPool.h"
#include "../instruction/InstructionSequence.h"

class Bytecode {
private:
    Bytecode() = default;
    std::map<std::uint64_t, std::uint64_t> functionMemoryUseMap;
    std::vector<std::uint8_t> codeArea;
    std::vector<std::uint8_t> dataArea;

public:
    void outputToBinaryFile(std::unique_ptr<std::ofstream> file);
    void outputToHumanReadableFile(std::unique_ptr<std::ofstream> file);
    [[nodiscard]] const std::map<std::uint64_t, std::uint64_t> &getMemoryUseMap() const;
    [[nodiscard]] const std::vector<std::uint8_t> &getCodeArea() const;
    [[nodiscard]] const std::vector<std::uint8_t> &getDataArea() const;
    static Bytecode *build(SymbolTable *symbolTable, StringConstantPool *stringConstantPool, InstructionSequence *instructionSequence);
    static Bytecode *build(std::unique_ptr<std::ifstream> file);
};
