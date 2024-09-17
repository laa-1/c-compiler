#pragma once

#include <cstdint>
#include "../Expression.h"

class IntegerLiteralExpression : public Expression {
public:
    int value;

    IntegerLiteralExpression(int lineNumber, int columnNumber, int value) : Expression(lineNumber, columnNumber), value(value) {}

    ~IntegerLiteralExpression() override = default;

    ExpressionClass getClass() override {
        return ExpressionClass::INT_LITERAL_EXPRESSION;
    }

};
