#pragma once

#include <map>
#include <vector>
#include <stack>
#include <string>
#include "../bytecode/Bytecode.h"
#include "../instruction/Instruction.h"

/**
 * 操作数栈的元素。
 * 大小为64比特。
 * 利用union实现比特层面的重解释转换。
 */
union OperandStackElem {
    std::int64_t i64;
    std::uint64_t u64 = 0;
    double f64;

    explicit OperandStackElem(std::int64_t i64) : i64(i64) {}
    explicit OperandStackElem(std::uint64_t u64) : u64(u64) {}
    explicit OperandStackElem(double f64) : f64(f64) {}
};

/**
 * 虚拟机类。
 * 栈式虚拟机，所有计算操作均在操作数栈进行。
 * 内存分为两块，代码区和数据区，其地址也是独立计算的。
 */
class VirtualMachine {
private:
    std::map<std::uint64_t, std::uint64_t> memoryUseMap;
    std::vector<std::uint8_t> codeArea;
    std::vector<std::uint8_t> dataArea;
    std::uint64_t pc; // 下一条指令的地址
    std::uint64_t bp; // 基地址
    std::stack<OperandStackElem> operandStack;
    std::stack<std::uint64_t> callAddressStack;
    std::stack<std::uint64_t> returnAddressStack;

private:
    Instruction fetchInstruction();
    void execute(const Instruction &instruction);

public:
    explicit VirtualMachine(Bytecode *bytecode);
    void run();
};
