#pragma once

#include <cstdint>
#include "../Expression.h"

class IntegerLiteralExpression : public Expression {
public:
    int value;

    IntegerLiteralExpression(int lineNumber, int columnNumber, int value);
    ~IntegerLiteralExpression() override = default;
    ExpressionClass getClass() override;
};
