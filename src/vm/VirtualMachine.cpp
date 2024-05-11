#include "VirtualMachine.h"

#include <iostream>
#include <cstring>
#include "../logger/Logger.h"

VirtualMachine::VirtualMachine(Bytecode *bytecode) {
    memoryUseMap = bytecode->getMemoryUseMap();
    codeArea.insert(codeArea.end(), bytecode->getCodeArea().begin(), bytecode->getCodeArea().end());
    dataArea.insert(dataArea.end(), bytecode->getDataArea().begin(), bytecode->getDataArea().end());
    dataArea.insert(dataArea.end(), 1024 * 1024, 0);
    pc = 0;
    bp = 0;
    callAddressStack.push(0);
}

Instruction VirtualMachine::fetchInstruction() {
    Opcode opcode;
    std::memcpy(&opcode, &codeArea[pc], sizeof(opcode));
    if (opcode == Opcode::PUSH_64) {
        std::uint64_t operand = 0;
        std::memcpy(&operand, &codeArea[pc + 2], sizeof(operand));
        pc += 10;
        return {opcode, operand};
    } else {
        pc += 2;
        return {opcode, static_cast<std::uint64_t>(0)};
    }
}

void VirtualMachine::execute(const Instruction &instruction) {
    switch (instruction.opcode) {
        case Opcode::ADD_I64: {
            auto rightValue = operandStack.top().i64;
            operandStack.pop();
            auto leftValue = operandStack.top().i64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::int64_t>(leftValue + rightValue));
            break;
        }
        case Opcode::ADD_U64: {
            auto rightValue = operandStack.top().u64;
            operandStack.pop();
            auto leftValue = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue + rightValue));
            break;
        }
        case Opcode::ADD_F64: {
            auto rightValue = operandStack.top().f64;
            operandStack.pop();
            auto leftValue = operandStack.top().f64;
            operandStack.pop();
            operandStack.emplace(static_cast<double>(leftValue + rightValue));
            break;
        }
        case Opcode::SUB_I64: {
            auto rightValue = operandStack.top().i64;
            operandStack.pop();
            auto leftValue = operandStack.top().i64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::int64_t>(leftValue - rightValue));
            break;
        }
        case Opcode::SUB_U64: {
            auto rightValue = operandStack.top().u64;
            operandStack.pop();
            auto leftValue = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue - rightValue));
            break;
        }
        case Opcode::SUB_F64: {
            auto rightValue = operandStack.top().f64;
            operandStack.pop();
            auto leftValue = operandStack.top().f64;
            operandStack.pop();
            operandStack.emplace(static_cast<double>(leftValue - rightValue));
            break;
        }
        case Opcode::MUL_I64: {
            auto rightValue = operandStack.top().i64;
            operandStack.pop();
            auto leftValue = operandStack.top().i64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::int64_t>(leftValue * rightValue));
            break;
        }
        case Opcode::MUL_U64: {
            auto rightValue = operandStack.top().u64;
            operandStack.pop();
            auto leftValue = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue * rightValue));
            break;
        }
        case Opcode::MUL_F64: {
            auto rightValue = operandStack.top().f64;
            operandStack.pop();
            auto leftValue = operandStack.top().f64;
            operandStack.pop();
            operandStack.emplace(static_cast<double>(leftValue * rightValue));
            break;
        }
        case Opcode::DIV_I64: {
            auto rightValue = operandStack.top().i64;
            operandStack.pop();
            auto leftValue = operandStack.top().i64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::int64_t>(leftValue / rightValue));
            break;
        }
        case Opcode::DIV_U64: {
            auto rightValue = operandStack.top().u64;
            operandStack.pop();
            auto leftValue = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue / rightValue));
            break;
        }
        case Opcode::DIV_F64: {
            auto rightValue = operandStack.top().f64;
            operandStack.pop();
            auto leftValue = operandStack.top().f64;
            operandStack.pop();
            operandStack.emplace(static_cast<double>(leftValue / rightValue));
            break;
        }
        case Opcode::MOD_I64: {
            auto rightValue = operandStack.top().i64;
            operandStack.pop();
            auto leftValue = operandStack.top().i64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::int64_t>(leftValue % rightValue));
            break;
        }
        case Opcode::MOD_U64: {
            auto rightValue = operandStack.top().u64;
            operandStack.pop();
            auto leftValue = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue % rightValue));
            break;
        }
        case Opcode::NEG_I64: {
            auto value = operandStack.top().i64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::int64_t>(-value));
            break;
        }
        case Opcode::NEG_F64: {
            auto value = operandStack.top().f64;
            operandStack.pop();
            operandStack.emplace(static_cast<double>(-value));
            break;
        }
        case Opcode::SL_I64: {
            auto rightValue = operandStack.top().i64;
            operandStack.pop();
            auto leftValue = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::int64_t>(leftValue << rightValue));
            break;
        }
        case Opcode::SL_U64: {
            auto rightValue = operandStack.top().u64;
            operandStack.pop();
            auto leftValue = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue << rightValue));
            break;
        }
        case Opcode::SR_I64: {
            auto rightValue = operandStack.top().i64;
            operandStack.pop();
            auto leftValue = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::int64_t>(leftValue >> rightValue));
            break;
        }
        case Opcode::SR_U64: {
            auto rightValue = operandStack.top().u64;
            operandStack.pop();
            auto leftValue = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue >> rightValue));
            break;
        }
        case Opcode::AND_64: {
            auto rightValue = operandStack.top().u64;
            operandStack.pop();
            auto leftValue = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue & rightValue));
            break;
        }
        case Opcode::OR_64: {
            auto rightValue = operandStack.top().u64;
            operandStack.pop();
            auto leftValue = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue | rightValue));
            break;
        }
        case Opcode::NOT_64: {
            auto value = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(~value));
            break;
        }
        case Opcode::XOR_64: {
            auto rightValue = operandStack.top().u64;
            operandStack.pop();
            auto leftValue = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue ^ rightValue));
            break;
        }
        case Opcode::TB_64: {
            auto value = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(value != static_cast<std::uint64_t>(0)));
            break;
        }
        case Opcode::GT_I64: {
            auto rightValue = operandStack.top().i64;
            operandStack.pop();
            auto leftValue = operandStack.top().i64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue > rightValue));
            break;
        }
        case Opcode::GT_U64: {
            auto rightValue = operandStack.top().u64;
            operandStack.pop();
            auto leftValue = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue > rightValue));
            break;
        }
        case Opcode::GT_F64: {
            auto rightValue = operandStack.top().f64;
            operandStack.pop();
            auto leftValue = operandStack.top().f64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue > rightValue));
            break;
        }
        case Opcode::LT_I64: {
            auto rightValue = operandStack.top().i64;
            operandStack.pop();
            auto leftValue = operandStack.top().i64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue < rightValue));
            break;
        }
        case Opcode::LT_U64: {
            auto rightValue = operandStack.top().u64;
            operandStack.pop();
            auto leftValue = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue < rightValue));
            break;
        }
        case Opcode::LT_F64: {
            auto rightValue = operandStack.top().f64;
            operandStack.pop();
            auto leftValue = operandStack.top().f64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue < rightValue));
            break;
        }
        case Opcode::EQ_I64: {
            auto rightValue = operandStack.top().i64;
            operandStack.pop();
            auto leftValue = operandStack.top().i64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue == rightValue));
            break;
        }
        case Opcode::EQ_U64: {
            auto rightValue = operandStack.top().u64;
            operandStack.pop();
            auto leftValue = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue == rightValue));
            break;
        }
        case Opcode::EQ_F64: {
            auto rightValue = operandStack.top().f64;
            operandStack.pop();
            auto leftValue = operandStack.top().f64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(leftValue == rightValue));
            break;
        }
        case Opcode::CAST_I64_U64: {
            auto value = operandStack.top().i64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(value));
            break;
        }
        case Opcode::CAST_I64_F64: {
            auto value = operandStack.top().i64;
            operandStack.pop();
            operandStack.emplace(static_cast<double>(value));
            break;
        }
        case Opcode::CAST_U64_I64: {
            auto value = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::int64_t>(value));
            break;
        }
        case Opcode::CAST_U64_F64: {
            auto value = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<double>(value));
            break;
        }
        case Opcode::CAST_F64_I64: {
            auto value = operandStack.top().f64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::int64_t>(value));
            break;
        }
        case Opcode::CAST_F64_U64: {
            auto value = operandStack.top().f64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(value));
            break;
        }
        case Opcode::JMP: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            pc = address;
            break;
        }
        case Opcode::JZ_64: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            auto value = operandStack.top().u64;
            operandStack.pop();
            if (value == static_cast<std::uint64_t>(0)) {
                pc = address;
            }
            break;
        }
        case Opcode::JNZ_64: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            auto value = operandStack.top().u64;
            operandStack.pop();
            if (value != static_cast<std::uint64_t>(0)) {
                pc = address;
            }
            break;
        }
        case Opcode::LOAD_I8: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            std::int8_t loadValue;
            std::memcpy(&loadValue, &dataArea[address], sizeof(loadValue));
            operandStack.emplace(static_cast<std::int64_t>(loadValue));
            break;
        }
        case Opcode::LOAD_I16: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            std::int16_t loadValue;
            std::memcpy(&loadValue, &dataArea[address], sizeof(loadValue));
            operandStack.emplace(static_cast<std::int64_t>(loadValue));
            break;
        }
        case Opcode::LOAD_I32: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            std::int32_t loadValue;
            std::memcpy(&loadValue, &dataArea[address], sizeof(loadValue));
            operandStack.emplace(static_cast<std::int64_t>(loadValue));
            break;
        }
        case Opcode::LOAD_I64: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            std::int64_t loadValue;
            std::memcpy(&loadValue, &dataArea[address], sizeof(loadValue));
            operandStack.emplace(static_cast<std::int64_t>(loadValue));
            break;
        }
        case Opcode::LOAD_U8: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            std::uint8_t loadValue;
            std::memcpy(&loadValue, &dataArea[address], sizeof(loadValue));
            operandStack.emplace(static_cast<std::uint64_t>(loadValue));
            break;
        }
        case Opcode::LOAD_U16: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            std::uint16_t loadValue;
            std::memcpy(&loadValue, &dataArea[address], sizeof(loadValue));
            operandStack.emplace(static_cast<std::uint64_t>(loadValue));
            break;
        }
        case Opcode::LOAD_U32: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            std::uint32_t loadValue;
            std::memcpy(&loadValue, &dataArea[address], sizeof(loadValue));
            operandStack.emplace(static_cast<std::uint64_t>(loadValue));
            break;
        }
        case Opcode::LOAD_U64: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            std::uint64_t loadValue;
            std::memcpy(&loadValue, &dataArea[address], sizeof(loadValue));
            operandStack.emplace(static_cast<std::uint64_t>(loadValue));
            break;
        }
        case Opcode::LOAD_F32: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            float loadValue;
            std::memcpy(&loadValue, &dataArea[address], sizeof(loadValue));
            operandStack.emplace(static_cast<double>(loadValue));
            break;
        }
        case Opcode::LOAD_F64: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            double loadValue;
            std::memcpy(&loadValue, &dataArea[address], sizeof(loadValue));
            operandStack.emplace(static_cast<double>(loadValue));
            break;
        }
        case Opcode::STORE_I8: {
            auto value = operandStack.top().i64;
            operandStack.pop();
            auto address = operandStack.top().u64;
            operandStack.pop();
            auto storeValue = static_cast<std::int8_t>(value);
            std::memcpy(&dataArea[address], &storeValue, sizeof(storeValue));
            break;
        }
        case Opcode::STORE_I16: {
            auto value = operandStack.top().i64;
            operandStack.pop();
            auto address = operandStack.top().u64;
            operandStack.pop();
            auto storeValue = static_cast<std::int16_t>(value);
            std::memcpy(&dataArea[address], &storeValue, sizeof(storeValue));
            break;
        }
        case Opcode::STORE_I32: {
            auto value = operandStack.top().i64;
            operandStack.pop();
            auto address = operandStack.top().u64;
            operandStack.pop();
            auto storeValue = static_cast<std::int32_t>(value);
            std::memcpy(&dataArea[address], &storeValue, sizeof(storeValue));
            break;
        }
        case Opcode::STORE_I64: {
            auto value = operandStack.top().i64;
            operandStack.pop();
            auto address = operandStack.top().u64;
            operandStack.pop();
            auto storeValue = static_cast<std::int64_t>(value);
            std::memcpy(&dataArea[address], &storeValue, sizeof(storeValue));
            break;
        }
        case Opcode::STORE_U8: {
            auto value = operandStack.top().u64;
            operandStack.pop();
            auto address = operandStack.top().u64;
            operandStack.pop();
            auto storeValue = static_cast<std::uint8_t>(value);
            std::memcpy(&dataArea[address], &storeValue, sizeof(storeValue));
            break;
        }
        case Opcode::STORE_U16: {
            auto value = operandStack.top().u64;
            operandStack.pop();
            auto address = operandStack.top().u64;
            operandStack.pop();
            auto storeValue = static_cast<std::uint16_t>(value);
            std::memcpy(&dataArea[address], &storeValue, sizeof(storeValue));
            break;
        }
        case Opcode::STORE_U32: {
            auto value = operandStack.top().u64;
            operandStack.pop();
            auto address = operandStack.top().u64;
            operandStack.pop();
            auto storeValue = static_cast<std::uint32_t>(value);
            std::memcpy(&dataArea[address], &storeValue, sizeof(storeValue));
            break;
        }
        case Opcode::STORE_U64: {
            auto value = operandStack.top().u64;
            operandStack.pop();
            auto address = operandStack.top().u64;
            operandStack.pop();
            auto storeValue = static_cast<std::uint64_t>(value);
            std::memcpy(&dataArea[address], &storeValue, sizeof(storeValue));
            break;
        }
        case Opcode::STORE_F32: {
            auto value = operandStack.top().f64;
            operandStack.pop();
            auto address = operandStack.top().u64;
            operandStack.pop();
            auto storeValue = static_cast<float>(value);
            std::memcpy(&dataArea[address], &storeValue, sizeof(storeValue));
            break;
        }
        case Opcode::STORE_F64: {
            auto value = operandStack.top().f64;
            operandStack.pop();
            auto address = operandStack.top().u64;
            operandStack.pop();
            auto storeValue = static_cast<double>(value);
            std::memcpy(&dataArea[address], &storeValue, sizeof(storeValue));
            break;
        }
        case Opcode::IN_I64: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            std::int64_t input;
            std::cin >> input;
            std::memcpy(&dataArea[address], &input, sizeof(input));
            break;
        }
        case Opcode::IN_U64: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            std::uint64_t input;
            std::cin >> input;
            std::memcpy(&dataArea[address], &input, sizeof(input));
            break;
        }
        case Opcode::IN_F64: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            double input;
            std::cin >> input;
            std::memcpy(&dataArea[address], &input, sizeof(input));
            break;
        }
        case Opcode::IN_S: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            std::cin.getline(reinterpret_cast<char *>(&dataArea[address]), (std::streamsize)(dataArea.size() - address));
            break;
        }
        case Opcode::OUT_I64: {
            auto value = operandStack.top().i64;
            operandStack.pop();
            std::cout << value;
            break;
        }
        case Opcode::OUT_U64: {
            auto value = operandStack.top().u64;
            operandStack.pop();
            std::cout << value;
            break;
        }
        case Opcode::OUT_F64: {
            auto value = operandStack.top().f64;
            operandStack.pop();
            std::cout << value;
            break;
        }
        case Opcode::OUT_S: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            std::cout << reinterpret_cast<const char *>(&dataArea[address]);
            break;
        }
        case Opcode::CALL: {
            auto address = operandStack.top().u64;
            operandStack.pop();
            bp += memoryUseMap[callAddressStack.top()];
            returnAddressStack.push(pc);
            pc = address;
            callAddressStack.push(pc);
            break;
        }
        case Opcode::RET: {
            callAddressStack.pop();
            pc = returnAddressStack.top();
            returnAddressStack.pop();
            bp -= memoryUseMap[callAddressStack.top()];
            break;
        }
        case Opcode::PUSH_64: {
            operandStack.emplace(static_cast<std::uint64_t>(instruction.operand.u64));
            break;
        }
        case Opcode::POP_64: {
            operandStack.pop();
            break;
        }
        case Opcode::COPY_64: {
            auto value = operandStack.top().u64;
            operandStack.emplace(static_cast<std::uint64_t>(value));
            break;
        }
        case Opcode::SWAP_64: {
            auto value1 = operandStack.top().u64;
            operandStack.pop();
            auto value2 = operandStack.top().u64;
            operandStack.pop();
            operandStack.emplace(static_cast<std::uint64_t>(value1));
            operandStack.emplace(static_cast<std::uint64_t>(value2));
            break;
        }
        case Opcode::FBP: {
            operandStack.emplace(static_cast<std::uint64_t>(bp));
            break;
        }
        case Opcode::HLT: {
            break;
        }
        default: {
            Logger::error("invalid instruction opcode: " + std::to_string(static_cast<short>(instruction.opcode)));
            exit(1);
        }
    }
}

void VirtualMachine::run() {
    while (true) {
        Instruction instruction = fetchInstruction();
        if (instruction.opcode == Opcode::HLT) {
            break;
        }
        execute(instruction);
    }
}
