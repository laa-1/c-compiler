#pragma once

#include <vector>
#include "Instruction.h"
#include "BinaryDataType.h"
#include "InstructionSequence.h"

/**
 * 用于构建InstructionSequence的Builder类
 * 封装同一功能的指令有针对不同数据类型的版本的问题
 */
class InstructionSequenceBuilder {
private:
    std::vector<Instruction> instructionList;
    std::uint64_t nextInstructionAddress = 0;

public:
    void appendAdd(BinaryDataType binaryDataType);
    void appendSub(BinaryDataType binaryDataType);
    void appendMul(BinaryDataType binaryDataType);
    void appendDiv(BinaryDataType binaryDataType);
    void appendMod(BinaryDataType binaryDataType);
    void appendNeg(BinaryDataType binaryDataType);
    void appendSl(BinaryDataType binaryDataType);
    void appendSr(BinaryDataType binaryDataType);
    void appendAnd();
    void appendOr();
    void appendNot();
    void appendXor();
    void appendTb();
    void appendGt(BinaryDataType binaryDataType);
    void appendLt(BinaryDataType binaryDataType);
    void appendEq(BinaryDataType binaryDataType);
    void appendCast(BinaryDataType sourceBinaryDataType, BinaryDataType targetBinaryDataType);
    void appendJmp();
    void appendJz();
    void appendJnz();
    void appendLoad(BinaryDataType binaryDataType);
    void appendStore(BinaryDataType binaryDataType);
    void appendIn(BinaryDataType binaryDataType);
    void appendIn();
    void appendOut(BinaryDataType binaryDataType);
    void appendOut();
    void appendCall();
    void appendRet();
    void appendPush(std::int8_t value);
    void appendPush(std::int16_t value);
    void appendPush(std::int32_t value);
    void appendPush(std::int64_t value);
    void appendPush(std::uint8_t value);
    void appendPush(std::uint16_t value);
    void appendPush(std::uint32_t value);
    void appendPush(std::uint64_t value);
    void appendPush(float value);
    void appendPush(double value);
    void appendPop();
    void appendCopy();
    void appendSwap();
    void appendFbp();
    void appendHlt();
    void modifyPush(int instructionIndex, std::int8_t value);
    void modifyPush(int instructionIndex, std::int16_t value);
    void modifyPush(int instructionIndex, std::int32_t value);
    void modifyPush(int instructionIndex, std::int64_t value);
    void modifyPush(int instructionIndex, std::uint8_t value);
    void modifyPush(int instructionIndex, std::uint16_t value);
    void modifyPush(int instructionIndex, std::uint32_t value);
    void modifyPush(int instructionIndex, std::uint64_t value);
    Instruction getLastInstruction();
    int getNextInstructionIndex();
    std::uint64_t getNextInstructionAddress() const;
    InstructionSequence *build();
};
