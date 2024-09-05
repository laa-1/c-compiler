#pragma once

#include "../Expression.h"

class FloatingPointLiteralExpression : public Expression {
public:
    double value;

    FloatingPointLiteralExpression(int lineNumber, int columnNumber, double value);
    ~FloatingPointLiteralExpression() override = default;
    ExpressionClass getClass() override;
};
