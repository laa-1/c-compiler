#pragma once

#include <vector>
#include "Instruction.h"

class InstructionSequence {
private:
    std::vector<Instruction> instructionList;

public:
    explicit InstructionSequence(const std::vector<Instruction> &instructionList);
    // 序列化为字节数组形式，作为运行时的代码区
    std::vector<std::uint8_t> serialize();
};
