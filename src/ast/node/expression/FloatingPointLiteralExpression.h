#pragma once

#include "../Expression.h"

class FloatingPointLiteralExpression : public Expression {
public:
    double value;

    explicit FloatingPointLiteralExpression(double value);
    ~FloatingPointLiteralExpression() override = default;
    ExpressionClass getClass() override;
    Expression *clone() override;
};
