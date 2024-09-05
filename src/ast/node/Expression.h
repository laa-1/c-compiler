#pragma once

#include <string>
#include <cstdint>
#include "Node.h"
#include "../visitor/Visitor.h"

enum class UnaryOperator {
    PREINCREMENT,
    PREDECREMENT,
    POSTINCREMENT,
    POSTDECREMENT,
    TAKE_ADDRESS,
    DEREFERENCE,
    PLUS,
    MINUS,
    BITWISE_NOT,
    LOGICAL_NOT,
    SIZEOF
};

enum class BinaryOperator {
    SUBSCRIPT,
    MUL,
    DIV,
    ADD,
    SUB,
    MOD,
    SHIFT_LEFT,
    SHIFT_RIGHT,
    BITWISE_AND,
    BITWISE_XOR,
    BITWISE_OR,
    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL,
    EQUAL,
    NOT_EQUAL,
    LOGICAL_AND,
    LOGICAL_OR,
    ASSIGN,
    MUL_ASSIGN,
    DIV_ASSIGN,
    ADD_ASSIGN,
    SUB_ASSIGN,
    MOD_ASSIGN,
    SHIFT_LEFT_ASSIGN,
    SHIFT_RIGHT_ASSIGN,
    BITWISE_AND_ASSIGN,
    BITWISE_XOR_ASSIGN,
    BITWISE_OR_ASSIGN,
    COMMA
};

enum class TernaryOperator {
    CONDITION
};

enum class ExpressionClass {
    BINARY_EXPRESSION,
    CALL_EXPRESSION,
    CAST_EXPRESSION,
    CHAR_LITERAL_EXPRESSION,
    FLOAT_LITERAL_EXPRESSION,
    IDENTIFIER_EXPRESSION,
    INT_LITERAL_EXPRESSION,
    STRING_LITERAL_EXPRESSION,
    TERNARY_EXPRESSION,
    UNARY_EXPRESSION,
};


class Expression : public Node {
public:
    Type *resultType = nullptr; // 计算结果的类型
    bool isLvalue = false; // 计算结果是否为左值

    Expression(int lineNumber, int columnNumber);
    ~Expression() override = default;
    void accept(Visitor *visitor) override;
    virtual ExpressionClass getClass() = 0;
};