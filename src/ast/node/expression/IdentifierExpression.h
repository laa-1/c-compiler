#pragma once

#include "../Expression.h"

class IdentifierExpression : public Expression {
public:
    std::string identifier;

    explicit IdentifierExpression(std::string identifier);
    ~IdentifierExpression() override = default;
    ExpressionClass getClass() override;
    Expression *clone() override;
};
