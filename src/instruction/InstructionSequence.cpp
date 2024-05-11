#include "InstructionSequence.h"
#include <cstring>

InstructionSequence::InstructionSequence(const std::vector<Instruction> &instructionList) : instructionList(instructionList) {}

std::vector<std::uint8_t> InstructionSequence::serialize() {
    std::vector<std::uint8_t> byteList(instructionList.size() * 10, 0);
    int i = 0;
    for (auto instruction : instructionList) {
        std::memcpy(&byteList[i], &instruction.opcode, sizeof(instruction.opcode));
        if (instruction.opcode == Opcode::PUSH_64) {
            std::memcpy(&byteList[i + 2], &instruction.operand, sizeof(instruction.operand));
            i += 10;
        } else {
            i += 2;
        }
    }
    byteList.resize(i);
    return byteList;
}
