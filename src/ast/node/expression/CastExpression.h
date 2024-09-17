#pragma once

#include "../Expression.h"
#include "../Type.h"
#include <memory>

class CastExpression : public Expression {
public:
    Type *targetType;
    Expression *operand;

    CastExpression(int lineNumber, int columnNumber, Type *targetType, Expression *operand) : Expression(lineNumber, columnNumber), targetType(targetType), operand(operand) {}

    ~CastExpression() override {
        delete targetType;
        delete operand;
    }

    ExpressionClass getClass() override {
        return ExpressionClass::CAST_EXPRESSION;
    }
};
