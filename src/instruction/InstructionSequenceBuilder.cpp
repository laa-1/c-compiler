#include "InstructionSequenceBuilder.h"

#include <cassert>

void InstructionSequenceBuilder::appendAdd(BinaryDataType binaryDataType) {
    switch (binaryDataType) {
        case BinaryDataType::I8:
        case BinaryDataType::I16:
        case BinaryDataType::I32:
        case BinaryDataType::I64:
            instructionList.emplace_back(Opcode::ADD_I64);
            break;
        case BinaryDataType::U8:
        case BinaryDataType::U16:
        case BinaryDataType::U32:
        case BinaryDataType::U64:
            instructionList.emplace_back(Opcode::ADD_U64);
            break;
        case BinaryDataType::F32:
        case BinaryDataType::F64:
            instructionList.emplace_back(Opcode::ADD_F64);
            break;
    }
}

void InstructionSequenceBuilder::appendSub(BinaryDataType binaryDataType) {
    switch (binaryDataType) {
        case BinaryDataType::I8:
        case BinaryDataType::I16:
        case BinaryDataType::I32:
        case BinaryDataType::I64:
            instructionList.emplace_back(Opcode::SUB_I64);
            break;
        case BinaryDataType::U8:
        case BinaryDataType::U16:
        case BinaryDataType::U32:
        case BinaryDataType::U64:
            instructionList.emplace_back(Opcode::SUB_U64);
            break;
        case BinaryDataType::F32:
        case BinaryDataType::F64:
            instructionList.emplace_back(Opcode::SUB_F64);
            break;
    }
}

void InstructionSequenceBuilder::appendMul(BinaryDataType binaryDataType) {
    switch (binaryDataType) {
        case BinaryDataType::I8:
        case BinaryDataType::I16:
        case BinaryDataType::I32:
        case BinaryDataType::I64:
            instructionList.emplace_back(Opcode::MUL_I64);
            break;
        case BinaryDataType::U8:
        case BinaryDataType::U16:
        case BinaryDataType::U32:
        case BinaryDataType::U64:
            instructionList.emplace_back(Opcode::MUL_U64);
            break;
        case BinaryDataType::F32:
        case BinaryDataType::F64:
            instructionList.emplace_back(Opcode::MUL_F64);
            break;
    }
}

void InstructionSequenceBuilder::appendDiv(BinaryDataType binaryDataType) {
    switch (binaryDataType) {
        case BinaryDataType::I8:
        case BinaryDataType::I16:
        case BinaryDataType::I32:
        case BinaryDataType::I64:
            instructionList.emplace_back(Opcode::DIV_I64);
            break;
        case BinaryDataType::U8:
        case BinaryDataType::U16:
        case BinaryDataType::U32:
        case BinaryDataType::U64:
            instructionList.emplace_back(Opcode::DIV_U64);
            break;
        case BinaryDataType::F32:
        case BinaryDataType::F64:
            instructionList.emplace_back(Opcode::DIV_F64);
            break;
    }
}

void InstructionSequenceBuilder::appendMod(BinaryDataType binaryDataType) {
    switch (binaryDataType) {
        case BinaryDataType::I8:
        case BinaryDataType::I16:
        case BinaryDataType::I32:
        case BinaryDataType::I64:
            instructionList.emplace_back(Opcode::MOD_I64);
            break;
        case BinaryDataType::U8:
        case BinaryDataType::U16:
        case BinaryDataType::U32:
        case BinaryDataType::U64:
            instructionList.emplace_back(Opcode::MOD_U64);
            break;
        case BinaryDataType::F32:
        case BinaryDataType::F64:
            assert(false);
    }
}

void InstructionSequenceBuilder::appendNeg(BinaryDataType binaryDataType) {
    switch (binaryDataType) {
        case BinaryDataType::I8:
        case BinaryDataType::I16:
        case BinaryDataType::I32:
        case BinaryDataType::I64:
            instructionList.emplace_back(Opcode::NEG_I64);
            break;
        case BinaryDataType::U8:
        case BinaryDataType::U16:
        case BinaryDataType::U32:
        case BinaryDataType::U64:
            assert(false);
        case BinaryDataType::F32:
        case BinaryDataType::F64:
            instructionList.emplace_back(Opcode::NEG_F64);
            break;
    }
}

void InstructionSequenceBuilder::appendSl(BinaryDataType binaryDataType) {
    switch (binaryDataType) {
        case BinaryDataType::I8:
        case BinaryDataType::I16:
        case BinaryDataType::I32:
        case BinaryDataType::I64:
            instructionList.emplace_back(Opcode::SL_I64);
            break;
        case BinaryDataType::U8:
        case BinaryDataType::U16:
        case BinaryDataType::U32:
        case BinaryDataType::U64:
            instructionList.emplace_back(Opcode::SL_U64);
            break;
        case BinaryDataType::F32:
        case BinaryDataType::F64:
            assert(false);
    }
}

void InstructionSequenceBuilder::appendSr(BinaryDataType binaryDataType) {
    switch (binaryDataType) {
        case BinaryDataType::I8:
        case BinaryDataType::I16:
        case BinaryDataType::I32:
        case BinaryDataType::I64:
            instructionList.emplace_back(Opcode::SR_I64);
            break;
        case BinaryDataType::U8:
        case BinaryDataType::U16:
        case BinaryDataType::U32:
        case BinaryDataType::U64:
            instructionList.emplace_back(Opcode::SR_U64);
            break;
        case BinaryDataType::F32:
        case BinaryDataType::F64:
            assert(false);
    }
}

void InstructionSequenceBuilder::appendAnd() {
    instructionList.emplace_back(Opcode::AND_64);
}

void InstructionSequenceBuilder::appendOr() {
    instructionList.emplace_back(Opcode::OR_64);
}

void InstructionSequenceBuilder::appendNot() {
    instructionList.emplace_back(Opcode::NOT_64);
}

void InstructionSequenceBuilder::appendXor() {
    instructionList.emplace_back(Opcode::XOR_64);
}

void InstructionSequenceBuilder::appendTb() {
    instructionList.emplace_back(Opcode::TB_64);
}

void InstructionSequenceBuilder::appendGt(BinaryDataType binaryDataType) {
    switch (binaryDataType) {
        case BinaryDataType::I8:
        case BinaryDataType::I16:
        case BinaryDataType::I32:
        case BinaryDataType::I64:
            instructionList.emplace_back(Opcode::GT_I64);
            break;
        case BinaryDataType::U8:
        case BinaryDataType::U16:
        case BinaryDataType::U32:
        case BinaryDataType::U64:
            instructionList.emplace_back(Opcode::GT_U64);
            break;
        case BinaryDataType::F32:
        case BinaryDataType::F64:
            instructionList.emplace_back(Opcode::GT_F64);
            break;
    }
}

void InstructionSequenceBuilder::appendLt(BinaryDataType binaryDataType) {
    switch (binaryDataType) {
        case BinaryDataType::I8:
        case BinaryDataType::I16:
        case BinaryDataType::I32:
        case BinaryDataType::I64:
            instructionList.emplace_back(Opcode::LT_I64);
            break;
        case BinaryDataType::U8:
        case BinaryDataType::U16:
        case BinaryDataType::U32:
        case BinaryDataType::U64:
            instructionList.emplace_back(Opcode::LT_U64);
            break;
        case BinaryDataType::F32:
        case BinaryDataType::F64:
            instructionList.emplace_back(Opcode::LT_F64);
            break;
    }
}

void InstructionSequenceBuilder::appendEq(BinaryDataType binaryDataType) {
    switch (binaryDataType) {
        case BinaryDataType::I8:
        case BinaryDataType::I16:
        case BinaryDataType::I32:
        case BinaryDataType::I64:
            instructionList.emplace_back(Opcode::EQ_I64);
            break;
        case BinaryDataType::U8:
        case BinaryDataType::U16:
        case BinaryDataType::U32:
        case BinaryDataType::U64:
            instructionList.emplace_back(Opcode::EQ_U64);
            break;
        case BinaryDataType::F32:
        case BinaryDataType::F64:
            instructionList.emplace_back(Opcode::EQ_F64);
            break;
    }
}

void InstructionSequenceBuilder::appendCast(BinaryDataType sourceBinaryDataType, BinaryDataType targetBinaryDataType) {
    if ((sourceBinaryDataType == BinaryDataType::I8 || sourceBinaryDataType == BinaryDataType::I16 || sourceBinaryDataType == BinaryDataType::I32 || sourceBinaryDataType == BinaryDataType::I64) &&
        (targetBinaryDataType == BinaryDataType::I8 || targetBinaryDataType == BinaryDataType::I16 || targetBinaryDataType == BinaryDataType::I32 || targetBinaryDataType == BinaryDataType::I64)) {
        return;
    }
    if ((sourceBinaryDataType == BinaryDataType::U8 || sourceBinaryDataType == BinaryDataType::U16 || sourceBinaryDataType == BinaryDataType::U32 || sourceBinaryDataType == BinaryDataType::U64) &&
        (targetBinaryDataType == BinaryDataType::U8 || targetBinaryDataType == BinaryDataType::U16 || targetBinaryDataType == BinaryDataType::U32 || targetBinaryDataType == BinaryDataType::U64)) {
        return;
    }
    if ((sourceBinaryDataType == BinaryDataType::F32 || sourceBinaryDataType == BinaryDataType::F64) &&
        (targetBinaryDataType == BinaryDataType::F32 || targetBinaryDataType == BinaryDataType::F64)) {
        return;
    }
    if ((sourceBinaryDataType == BinaryDataType::I8 || sourceBinaryDataType == BinaryDataType::I16 || sourceBinaryDataType == BinaryDataType::I32 || sourceBinaryDataType == BinaryDataType::I64) &&
        (targetBinaryDataType == BinaryDataType::U8 || targetBinaryDataType == BinaryDataType::U16 || targetBinaryDataType == BinaryDataType::U32 || targetBinaryDataType == BinaryDataType::U64)) {
        instructionList.emplace_back(Opcode::CAST_I64_U64);
    } else if ((sourceBinaryDataType == BinaryDataType::I8 || sourceBinaryDataType == BinaryDataType::I16 || sourceBinaryDataType == BinaryDataType::I32 || sourceBinaryDataType == BinaryDataType::I64) &&
               (targetBinaryDataType == BinaryDataType::F32 || targetBinaryDataType == BinaryDataType::F64)) {
        instructionList.emplace_back(Opcode::CAST_I64_F64);
    } else if ((sourceBinaryDataType == BinaryDataType::U8 || sourceBinaryDataType == BinaryDataType::U16 || sourceBinaryDataType == BinaryDataType::U32 || sourceBinaryDataType == BinaryDataType::U64) &&
               (targetBinaryDataType == BinaryDataType::I8 || targetBinaryDataType == BinaryDataType::I16 || targetBinaryDataType == BinaryDataType::I32 || targetBinaryDataType == BinaryDataType::I64)) {
        instructionList.emplace_back(Opcode::CAST_U64_I64);
    } else if ((sourceBinaryDataType == BinaryDataType::U8 || sourceBinaryDataType == BinaryDataType::U16 || sourceBinaryDataType == BinaryDataType::U32 || sourceBinaryDataType == BinaryDataType::U64) &&
               (targetBinaryDataType == BinaryDataType::F32 || targetBinaryDataType == BinaryDataType::F64)) {
        instructionList.emplace_back(Opcode::CAST_U64_F64);
    } else if ((sourceBinaryDataType == BinaryDataType::F32 || sourceBinaryDataType == BinaryDataType::F64) &&
               (targetBinaryDataType == BinaryDataType::I8 || targetBinaryDataType == BinaryDataType::I16 || targetBinaryDataType == BinaryDataType::I32 || targetBinaryDataType == BinaryDataType::I64)) {
        instructionList.emplace_back(Opcode::CAST_F64_I64);
    } else if ((sourceBinaryDataType == BinaryDataType::F32 || sourceBinaryDataType == BinaryDataType::F64) &&
               (targetBinaryDataType == BinaryDataType::U8 || targetBinaryDataType == BinaryDataType::U16 || targetBinaryDataType == BinaryDataType::U32 || targetBinaryDataType == BinaryDataType::U64)) {
        instructionList.emplace_back(Opcode::CAST_F64_U64);
    } else {
        assert(false);
    }
}

void InstructionSequenceBuilder::appendJmp() {
    instructionList.emplace_back(Opcode::JMP);
}

void InstructionSequenceBuilder::appendJz() {
    instructionList.emplace_back(Opcode::JZ_64);
}

void InstructionSequenceBuilder::appendJnz() {
    instructionList.emplace_back(Opcode::JNZ_64);
}

void InstructionSequenceBuilder::appendLoad(BinaryDataType binaryDataType) {
    switch (binaryDataType) {
        case BinaryDataType::I8:
            instructionList.emplace_back(Opcode::LOAD_I8);
            break;
        case BinaryDataType::I16:
            instructionList.emplace_back(Opcode::LOAD_I16);
            break;
        case BinaryDataType::I32:
            instructionList.emplace_back(Opcode::LOAD_I32);
            break;
        case BinaryDataType::I64:
            instructionList.emplace_back(Opcode::LOAD_I64);
            break;
        case BinaryDataType::U8:
            instructionList.emplace_back(Opcode::LOAD_U8);
            break;
        case BinaryDataType::U16:
            instructionList.emplace_back(Opcode::LOAD_U16);
            break;
        case BinaryDataType::U32:
            instructionList.emplace_back(Opcode::LOAD_U32);
            break;
        case BinaryDataType::U64:
            instructionList.emplace_back(Opcode::LOAD_U64);
            break;
        case BinaryDataType::F32:
            instructionList.emplace_back(Opcode::LOAD_F32);
            break;
        case BinaryDataType::F64:
            instructionList.emplace_back(Opcode::LOAD_F64);
            break;
    }
}

void InstructionSequenceBuilder::appendStore(BinaryDataType binaryDataType) {
    switch (binaryDataType) {
        case BinaryDataType::I8:
            instructionList.emplace_back(Opcode::STORE_I8);
            break;
        case BinaryDataType::I16:
            instructionList.emplace_back(Opcode::STORE_I16);
            break;
        case BinaryDataType::I32:
            instructionList.emplace_back(Opcode::STORE_I32);
            break;
        case BinaryDataType::I64:
            instructionList.emplace_back(Opcode::STORE_I64);
            break;
        case BinaryDataType::U8:
            instructionList.emplace_back(Opcode::STORE_U8);
            break;
        case BinaryDataType::U16:
            instructionList.emplace_back(Opcode::STORE_U16);
            break;
        case BinaryDataType::U32:
            instructionList.emplace_back(Opcode::STORE_U32);
            break;
        case BinaryDataType::U64:
            instructionList.emplace_back(Opcode::STORE_U64);
            break;
        case BinaryDataType::F32:
            instructionList.emplace_back(Opcode::STORE_F32);
            break;
        case BinaryDataType::F64:
            instructionList.emplace_back(Opcode::STORE_F64);
            break;
    }
}

void InstructionSequenceBuilder::appendIn(BinaryDataType binaryDataType) {
    switch (binaryDataType) {
        case BinaryDataType::I8:
        case BinaryDataType::I16:
        case BinaryDataType::I32:
        case BinaryDataType::I64:
            instructionList.emplace_back(Opcode::IN_I64);
            break;
        case BinaryDataType::U8:
        case BinaryDataType::U16:
        case BinaryDataType::U32:
        case BinaryDataType::U64:
            instructionList.emplace_back(Opcode::IN_U64);
            break;
        case BinaryDataType::F32:
        case BinaryDataType::F64:
            instructionList.emplace_back(Opcode::IN_F64);
            break;
    }
}

void InstructionSequenceBuilder::appendIn() {
    instructionList.emplace_back(Opcode::IN_S);
}

void InstructionSequenceBuilder::appendOut(BinaryDataType binaryDataType) {
    switch (binaryDataType) {
        case BinaryDataType::I8:
        case BinaryDataType::I16:
        case BinaryDataType::I32:
        case BinaryDataType::I64:
            instructionList.emplace_back(Opcode::OUT_I64);
            break;
        case BinaryDataType::U8:
        case BinaryDataType::U16:
        case BinaryDataType::U32:
        case BinaryDataType::U64:
            instructionList.emplace_back(Opcode::OUT_U64);
            break;
        case BinaryDataType::F32:
        case BinaryDataType::F64:
            instructionList.emplace_back(Opcode::OUT_F64);
            break;
    }
}

void InstructionSequenceBuilder::appendOut() {
    instructionList.emplace_back(Opcode::OUT_S);
}

void InstructionSequenceBuilder::appendCall() {
    instructionList.emplace_back(Opcode::CALL);
}

void InstructionSequenceBuilder::appendRet() {
    instructionList.emplace_back(Opcode::RET);
}

void InstructionSequenceBuilder::appendPush(std::int8_t value) {
    instructionList.emplace_back(Opcode::PUSH_64, value);
}

void InstructionSequenceBuilder::appendPush(std::int16_t value) {
    instructionList.emplace_back(Opcode::PUSH_64, value);
}

void InstructionSequenceBuilder::appendPush(std::int32_t value) {
    instructionList.emplace_back(Opcode::PUSH_64, value);
}

void InstructionSequenceBuilder::appendPush(std::int64_t value) {
    instructionList.emplace_back(Opcode::PUSH_64, value);
}

void InstructionSequenceBuilder::appendPush(std::uint8_t value) {
    instructionList.emplace_back(Opcode::PUSH_64, value);
}

void InstructionSequenceBuilder::appendPush(std::uint16_t value) {
    instructionList.emplace_back(Opcode::PUSH_64, value);
}

void InstructionSequenceBuilder::appendPush(std::uint32_t value) {
    instructionList.emplace_back(Opcode::PUSH_64, value);
}

void InstructionSequenceBuilder::appendPush(std::uint64_t value) {
    instructionList.emplace_back(Opcode::PUSH_64, value);
}

void InstructionSequenceBuilder::appendPush(float value) {
    instructionList.emplace_back(Opcode::PUSH_64, value);
}

void InstructionSequenceBuilder::appendPush(double value) {
    instructionList.emplace_back(Opcode::PUSH_64, value);
}

void InstructionSequenceBuilder::appendPop() {
    instructionList.emplace_back(Opcode::POP_64);
}

void InstructionSequenceBuilder::appendCopy() {
    instructionList.emplace_back(Opcode::COPY_64);
}

void InstructionSequenceBuilder::appendSwap() {
    instructionList.emplace_back(Opcode::SWAP_64);
}

void InstructionSequenceBuilder::appendFbp() {
    instructionList.emplace_back(Opcode::FBP);
}

void InstructionSequenceBuilder::appendHlt() {
    instructionList.emplace_back(Opcode::HLT);
}

void InstructionSequenceBuilder::modifyPush(int instructionIndex, std::int8_t value) {
    assert(instructionList[instructionIndex].opcode == Opcode::PUSH_64);
    instructionList[instructionIndex].operand.i8 = value;
}

void InstructionSequenceBuilder::modifyPush(int instructionIndex, std::int16_t value) {
    assert(instructionList[instructionIndex].opcode == Opcode::PUSH_64);
    instructionList[instructionIndex].operand.i16 = value;
}

void InstructionSequenceBuilder::modifyPush(int instructionIndex, std::int32_t value) {
    assert(instructionList[instructionIndex].opcode == Opcode::PUSH_64);
    instructionList[instructionIndex].operand.i32 = value;
}

void InstructionSequenceBuilder::modifyPush(int instructionIndex, std::int64_t value) {
    assert(instructionList[instructionIndex].opcode == Opcode::PUSH_64);
    instructionList[instructionIndex].operand.i64 = value;
}

void InstructionSequenceBuilder::modifyPush(int instructionIndex, std::uint8_t value) {
    assert(instructionList[instructionIndex].opcode == Opcode::PUSH_64);
    instructionList[instructionIndex].operand.u8 = value;
}

void InstructionSequenceBuilder::modifyPush(int instructionIndex, std::uint16_t value) {
    assert(instructionList[instructionIndex].opcode == Opcode::PUSH_64);
    instructionList[instructionIndex].operand.u16 = value;
}

void InstructionSequenceBuilder::modifyPush(int instructionIndex, std::uint32_t value) {
    assert(instructionList[instructionIndex].opcode == Opcode::PUSH_64);
    instructionList[instructionIndex].operand.u32 = value;
}

void InstructionSequenceBuilder::modifyPush(int instructionIndex, std::uint64_t value) {
    assert(instructionList[instructionIndex].opcode == Opcode::PUSH_64);
    instructionList[instructionIndex].operand.u64 = value;
}

Instruction InstructionSequenceBuilder::getLastInstruction() {
    return instructionList.back();
}

int InstructionSequenceBuilder::getNextInstructionIndex() {
    return (int) instructionList.size();
}

std::uint64_t InstructionSequenceBuilder::getNextInstructionAddress() const {
    return instructionList.size() * 10;
}

InstructionSequence *InstructionSequenceBuilder::build() {
    return new InstructionSequence(instructionList);
}
