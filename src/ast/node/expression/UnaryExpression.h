#pragma once

#include "../Expression.h"
#include <memory>

class UnaryExpression : public Expression {
public:
    UnaryOperator unaryOperator;
    Expression *operand;

    UnaryExpression(int lineNumber, int columnNumber, UnaryOperator unaryOperator, Expression *operand) : Expression(lineNumber, columnNumber), unaryOperator(unaryOperator), operand(operand) {}

    ~UnaryExpression() override {
        delete operand;
    }

    ExpressionClass getClass() override {
        return ExpressionClass::UNARY_EXPRESSION;
    }
};
