#pragma once

#include <cstdint>
#include "../Expression.h"

class IntegerLiteralExpression : public Expression {
public:
    int value;

    explicit IntegerLiteralExpression(int value);
    ~IntegerLiteralExpression() override = default;
    ExpressionClass getClass() override;
    Expression *clone() override;
};
