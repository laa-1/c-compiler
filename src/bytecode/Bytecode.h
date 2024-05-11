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
    std::map<std::uint64_t, std::uint64_t> memoryUseMap; // 每个函数的入口地址到内存使用的映射表
    std::vector<std::uint8_t> codeArea;
    std::vector<std::uint8_t> dataArea;

public:
    Bytecode() = default;
    Bytecode(SymbolTable *symbolTable, StringConstantPool *stringConstantPool, InstructionSequence *instructionSequence);
    void outputToFile(std::unique_ptr<std::ofstream> file);
    void loadFromFile(std::unique_ptr<std::ifstream> file);
    void outputToReadableFile(std::unique_ptr<std::ofstream> file);
    const std::map<std::uint64_t, std::uint64_t> &getMemoryUseMap() const;
    const std::vector<std::uint8_t> &getCodeArea() const;
    const std::vector<std::uint8_t> &getDataArea() const;
};
