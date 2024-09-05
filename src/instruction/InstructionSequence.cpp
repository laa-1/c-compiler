#include "InstructionSequence.h"
#include <cstring>

InstructionSequence::InstructionSequence(const std::vector<Instruction> &instructionList) : instructionList(instructionList) {}

std::vector<std::uint8_t> InstructionSequence::serialize() {
    std::vector<std::uint8_t> byteList(instructionList.size() * 10, 0);
    int address = 0;
    for (auto instruction : instructionList) {
        std::memcpy(&byteList[address], &instruction.opcode, sizeof(instruction.opcode));
        std::memcpy(&byteList[address + 2], &instruction.operand, sizeof(instruction.operand));
        address += 10;
    }
    return byteList;
}
