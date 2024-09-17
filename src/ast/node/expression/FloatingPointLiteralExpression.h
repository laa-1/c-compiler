#pragma once

#include "../Expression.h"

class FloatingPointLiteralExpression : public Expression {
public:
    double value;

    FloatingPointLiteralExpression(int lineNumber, int columnNumber, double value) : Expression(lineNumber, columnNumber), value(value) {}

    ~FloatingPointLiteralExpression() override = default;

    ExpressionClass getClass() override {
        return ExpressionClass::FLOAT_LITERAL_EXPRESSION;
    }
};
