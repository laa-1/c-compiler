#pragma once

#include "../Expression.h"
#include "../vector"

class StringLiteralExpression : public Expression {
public:
    std::string value;

    explicit StringLiteralExpression(std::string value);
    ~StringLiteralExpression() override = default;
    ExpressionClass getClass() override;
    Expression *clone() override;
};
