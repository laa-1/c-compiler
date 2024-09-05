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
union OperandStackUnit {
    std::int64_t i64;
    std::uint64_t u64 = 0;
    double f64;

    explicit OperandStackUnit(std::int64_t i64) : i64(i64) {}
    explicit OperandStackUnit(std::uint64_t u64) : u64(u64) {}
    explicit OperandStackUnit(double f64) : f64(f64) {}
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
    std::stack<OperandStackUnit> operandStack;
    std::stack<std::uint64_t> callAddressStack;
    std::stack<std::uint64_t> returnAddressStack;

private:
    explicit VirtualMachine(Bytecode *bytecode);
    void run();

public:
    static void run(Bytecode *bytecode);
};
