#include "Bytecode.h"

#include <cassert>
#include <fstream>
#include <cstring>
#include <iostream>
#include <iomanip>

std::string opcode2String(Opcode opcode) {
    switch (opcode) {
        case Opcode::ADD_I64:
            return "add_i64";
        case Opcode::ADD_U64:
            return "add_u64";
        case Opcode::ADD_F64:
            return "add_f64";
        case Opcode::SUB_I64:
            return "sub_i64";
        case Opcode::SUB_U64:
            return "sub_u64";
        case Opcode::SUB_F64:
            return "sub_f64";
        case Opcode::MUL_I64:
            return "mul_i64";
        case Opcode::MUL_U64:
            return "mul_u64";
        case Opcode::MUL_F64:
            return "mul_f64";
        case Opcode::DIV_I64:
            return "div_i64";
        case Opcode::DIV_U64:
            return "div_u64";
        case Opcode::DIV_F64:
            return "div_f64";
        case Opcode::MOD_I64:
            return "mod_i64";
        case Opcode::MOD_U64:
            return "mod_u64";
        case Opcode::NEG_I64:
            return "neg_i64";
        case Opcode::NEG_F64:
            return "neg_f64";
        case Opcode::SL_I64:
            return "sl_i64";
        case Opcode::SL_U64:
            return "sl_u64";
        case Opcode::SR_I64:
            return "sr_i64";
        case Opcode::SR_U64:
            return "sr_u64";
        case Opcode::AND_64:
            return "and_64";
        case Opcode::OR_64:
            return "or_64";
        case Opcode::NOT_64:
            return "not_64";
        case Opcode::XOR_64:
            return "xor_64";
        case Opcode::TB_64:
            return "tb_64";
        case Opcode::GT_I64:
            return "gt_i64";
        case Opcode::GT_U64:
            return "gt_u64";
        case Opcode::GT_F64:
            return "gt_f64";
        case Opcode::LT_I64:
            return "lt_i64";
        case Opcode::LT_U64:
            return "lt_u64";
        case Opcode::LT_F64:
            return "lt_f64";
        case Opcode::EQ_I64:
            return "eq_i64";
        case Opcode::EQ_U64:
            return "eq_u64";
        case Opcode::EQ_F64:
            return "eq_f64";
        case Opcode::CAST_I64_U64:
            return "cast_i64_u64";
        case Opcode::CAST_I64_F64:
            return "cast_i64_f64";
        case Opcode::CAST_U64_I64:
            return "cast_u64_i64";
        case Opcode::CAST_U64_F64:
            return "cast_u64_f64";
        case Opcode::CAST_F64_I64:
            return "cast_f64_i64";
        case Opcode::CAST_F64_U64:
            return "cast_f64_u64";
        case Opcode::JMP:
            return "jmp";
        case Opcode::JZ_64:
            return "jz_64";
        case Opcode::JNZ_64:
            return "jnz_64";
        case Opcode::LOAD_I8:
            return "load_i8";
        case Opcode::LOAD_I16:
            return "load_i16";
        case Opcode::LOAD_I32:
            return "load_i32";
        case Opcode::LOAD_I64:
            return "load_i64";
        case Opcode::LOAD_U8:
            return "load_u8";
        case Opcode::LOAD_U16:
            return "load_u16";
        case Opcode::LOAD_U32:
            return "load_u32";
        case Opcode::LOAD_U64:
            return "load_u64";
        case Opcode::LOAD_F32:
            return "load_f32";
        case Opcode::LOAD_F64:
            return "load_f64";
        case Opcode::STORE_I8:
            return "store_i8";
        case Opcode::STORE_I16:
            return "store_i16";
        case Opcode::STORE_I32:
            return "store_i32";
        case Opcode::STORE_I64:
            return "store_i64";
        case Opcode::STORE_U8:
            return "store_u8";
        case Opcode::STORE_U16:
            return "store_u16";
        case Opcode::STORE_U32:
            return "store_u32";
        case Opcode::STORE_U64:
            return "store_u64";
        case Opcode::STORE_F32:
            return "store_f32";
        case Opcode::STORE_F64:
            return "store_f64";
        case Opcode::IN_I64:
            return "in_i64";
        case Opcode::IN_U64:
            return "in_u64";
        case Opcode::IN_F64:
            return "in_f64";
        case Opcode::IN_S:
            return "in_s";
        case Opcode::OUT_I64:
            return "out_i64";
        case Opcode::OUT_U64:
            return "out_u64";
        case Opcode::OUT_F64:
            return "out_f64";
        case Opcode::OUT_S:
            return "out_s";
        case Opcode::CALL:
            return "call";
        case Opcode::RET:
            return "ret";
        case Opcode::PUSH_64:
            return "push_64";
        case Opcode::POP_64:
            return "pop_64";
        case Opcode::COPY_64:
            return "copy_64";
        case Opcode::SWAP_64:
            return "swap_64";
        case Opcode::FBP:
            return "fbp";
        case Opcode::HLT:
            return "hlt";
    }
    assert(false);
}

const std::map<std::uint64_t, std::uint64_t> &Bytecode::getMemoryUseMap() const {
    return functionMemoryUseMap;
}

const std::vector<std::uint8_t> &Bytecode::getCodeArea() const {
    return codeArea;
}

const std::vector<std::uint8_t> &Bytecode::getDataArea() const {
    return dataArea;
}

void Bytecode::outputToBinaryFile(std::unique_ptr<std::ofstream> file) {
    std::uint64_t functionMemoryUseMapSize = functionMemoryUseMap.size();
    std::uint64_t codeAreaByteSize = codeArea.size();
    std::uint64_t dataAreaByteSize = dataArea.size();
    file->write(reinterpret_cast<const char *>(&functionMemoryUseMapSize), sizeof(functionMemoryUseMapSize));
    file->write(reinterpret_cast<const char *>(&codeAreaByteSize), sizeof(codeAreaByteSize));
    file->write(reinterpret_cast<const char *>(&dataAreaByteSize), sizeof(dataAreaByteSize));
    for (auto pair : functionMemoryUseMap) {
        file->write(reinterpret_cast<const char *>(&pair.first), sizeof(pair.first));
        file->write(reinterpret_cast<const char *>(&pair.second), sizeof(pair.second));
    }
    for (auto byte : codeArea) {
        file->write(reinterpret_cast<const char *>(&byte), sizeof(byte));
    }
    for (auto byte : dataArea) {
        file->write(reinterpret_cast<const char *>(&byte), sizeof(byte));
    }
}

void Bytecode::outputToHumanReadableFile(std::unique_ptr<std::ofstream> file) {
    *file << "-----FUNCTION MEMORY USE MAP-----" << std::endl;
    for (auto pair : functionMemoryUseMap) {
        *file << std::setw(20) << std::left << pair.first << pair.second << std::endl;
    }
    *file << std::endl;
    *file << "-----CODE AREA-----" << std::endl;
    for (int i = 0; i < codeArea.size(); i += 10) {
        Opcode opcode;
        std::uint64_t operand;
        std::memcpy(&opcode, &codeArea[i], sizeof(opcode));
        std::memcpy(&operand, &codeArea[i + 2], sizeof(operand));
        *file << std::setw(20) << std::left << i << std::setw(20) << std::left << opcode2String(opcode) << operand << std::endl;
    }
    *file << std::endl;
    *file << "-----DATA AREA-----" << std::endl;
    for (auto byte : dataArea) {
        *file << (char) byte << " ";
    }
    *file << std::endl;
}

Bytecode *Bytecode::build(SymbolTable *symbolTable, StringConstantPool *stringConstantPool, InstructionSequence *instructionSequence) {
    auto *bytecode = new Bytecode();
    bytecode->functionMemoryUseMap = symbolTable->createFunctionMemoryUseMap();
    bytecode->functionMemoryUseMap[0] = 8 + stringConstantPool->getMemoryUse() + symbolTable->getMemoryUseRootScope();
    bytecode->codeArea = instructionSequence->serialize();
    std::vector<std::uint8_t> stringConstantArea = stringConstantPool->serialize();
    bytecode->dataArea = std::vector<std::uint8_t>(8, 0);
    bytecode->dataArea.insert(bytecode->dataArea.end(), stringConstantArea.begin(), stringConstantArea.end());
    return bytecode;
}

Bytecode *Bytecode::build(std::unique_ptr<std::ifstream> file) {
    auto *bytecode = new Bytecode();
    std::uint64_t functionMemoryUseMapSize;
    std::uint64_t codeAreaByteSize;
    std::uint64_t dataAreaByteSize;
    file->read(reinterpret_cast<char *>(&functionMemoryUseMapSize), sizeof(functionMemoryUseMapSize));
    file->read(reinterpret_cast<char *>(&codeAreaByteSize), sizeof(codeAreaByteSize));
    file->read(reinterpret_cast<char *>(&dataAreaByteSize), sizeof(dataAreaByteSize));
    std::pair<std::uint64_t, std::uint64_t> pair;
    std::uint8_t byte;
    for (int i = 0; i < functionMemoryUseMapSize; i++) {
        file->read(reinterpret_cast<char *>(&pair.first), sizeof(pair.first));
        file->read(reinterpret_cast<char *>(&pair.second), sizeof(pair.second));
        bytecode->functionMemoryUseMap.insert(pair);
    }
    for (int i = 0; i < codeAreaByteSize; i++) {
        file->read(reinterpret_cast<char *>(&byte), sizeof(byte));
        bytecode->codeArea.push_back(byte);
    }
    for (int i = 0; i < dataAreaByteSize; i++) {
        file->read(reinterpret_cast<char *>(&byte), sizeof(byte));
        bytecode->dataArea.push_back(byte);
    }
    return bytecode;
}
