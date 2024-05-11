#pragma once

#include <cstdint>

enum class Opcode : std::uint16_t {
    ADD_I64 = 1, // 右值出栈，左值出栈，相加，结果入栈
    ADD_U64,
    ADD_F64,
    SUB_I64, // 右值出栈，左值出栈，相减，结果入栈
    SUB_U64,
    SUB_F64,
    MUL_I64, // 右值出栈，左值出栈，相乘，结果入栈
    MUL_U64,
    MUL_F64,
    DIV_I64, // 右值出栈，左值出栈，相除，结果入栈
    DIV_U64,
    DIV_F64,
    MOD_I64, // 右值出栈，左值出栈，取模，结果入栈
    MOD_U64,
    NEG_I64, // 值出栈，取负，结果入栈
    NEG_F64,
    SL_I64, // 右值出栈，左值出栈，算术左移，结果入栈
    SL_U64, // 右值出栈，左值出栈，逻辑左移，结果入栈
    SR_I64, // 右值出栈，左值出栈，算术右移，结果入栈
    SR_U64, // 右值出栈，左值出栈，逻辑右移，结果入栈
    AND_64, // 右值出栈，左值出栈，按位与，结果入栈
    OR_64, // 右值出栈，左值出栈，按位或，结果入栈
    NOT_64, // 值出栈，按位非，结果入栈
    XOR_64, // 右值出栈，左值出栈，按位异或，结果入栈
    TB_64, // 值出栈，若非全0则置1，结果入栈
    GT_I64, // 右值出栈，左值出栈，大于则1入栈，否则0入栈
    GT_U64,
    GT_F64,
    LT_I64, // 右值出栈，左值出栈，小于则1入栈，否则0入栈
    LT_U64,
    LT_F64,
    EQ_I64, // 右值出栈，左值出栈，等于则1入栈，否则0入栈
    EQ_U64,
    EQ_F64,
    CAST_I64_U64, // 值出栈，i64转u64，结果入栈
    CAST_I64_F64,
    CAST_U64_I64,
    CAST_U64_F64,
    CAST_F64_I64,
    CAST_F64_U64,
    JMP, // 指令地址出栈，跳转
    JZ_64, // 指令地址出栈，值出栈，若全0则跳转
    JNZ_64, // 指令地址出栈，值出栈，若非全0则跳转
    LOAD_I8, // 内存地址出栈，加载，值入栈
    LOAD_I16,
    LOAD_I32,
    LOAD_I64,
    LOAD_U8,
    LOAD_U16,
    LOAD_U32,
    LOAD_U64,
    LOAD_F32,
    LOAD_F64,
    STORE_I8, // 值出栈，内存地址出栈，存储
    STORE_I16,
    STORE_I32,
    STORE_I64,
    STORE_U8,
    STORE_U16,
    STORE_U32,
    STORE_U64,
    STORE_F32,
    STORE_F64,
    IN_I64, // 地址出栈，输入
    IN_U64,
    IN_F64,
    IN_S,
    OUT_I64, // 值出栈，输出
    OUT_U64,
    OUT_F64,
    OUT_S, // 地址出栈，输出
    CALL, // 指令地址出栈，函数调用
    RET, // 函数返回
    PUSH_64, // 立即数入栈
    POP_64, // 弹出栈顶值
    COPY_64, // 复制栈顶值
    SWAP_64, // 交换栈顶的两个值
    FBP, // 基地址入栈
    HLT, // 停机
};

struct Instruction {
    Opcode opcode;
    union {
        std::int8_t i8;
        std::int16_t i16;
        std::int32_t i32;
        std::int64_t i64;
        std::uint8_t u8;
        std::uint16_t u16;
        std::uint32_t u32;
        std::uint64_t u64 = 0;
        float f32;
        double f64;
    } operand; // 只有push指令拥有操作数

    explicit Instruction(Opcode opcode) {
        this->opcode = opcode;
    }

    Instruction(Opcode opcode, std::int8_t operand) {
        this->opcode = opcode;
        this->operand.i8 = operand;
    }

    Instruction(Opcode opcode, std::int16_t operand) {
        this->opcode = opcode;
        this->operand.i16 = operand;
    }

    Instruction(Opcode opcode, std::int32_t operand) {
        this->opcode = opcode;
        this->operand.i32 = operand;
    }

    Instruction(Opcode opcode, std::int64_t operand) {
        this->opcode = opcode;
        this->operand.i64 = operand;
    }

    Instruction(Opcode opcode, std::uint8_t operand) {
        this->opcode = opcode;
        this->operand.u8 = operand;
    }

    Instruction(Opcode opcode, std::uint16_t operand) {
        this->opcode = opcode;
        this->operand.u16 = operand;
    }

    Instruction(Opcode opcode, std::uint32_t operand) {
        this->opcode = opcode;
        this->operand.u32 = operand;
    }

    Instruction(Opcode opcode, std::uint64_t operand) {
        this->opcode = opcode;
        this->operand.u64 = operand;
    }

    Instruction(Opcode opcode, float operand) {
        this->opcode = opcode;
        this->operand.f32 = operand;
    }

    Instruction(Opcode opcode, double operand) {
        this->opcode = opcode;
        this->operand.f64 = operand;
    }
};