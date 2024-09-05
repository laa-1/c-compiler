#pragma once

#include "../Expression.h"

class IdentifierExpression : public Expression {
public:
    std::string identifier;

    IdentifierExpression(int lineNumber, int columnNumber, const std::string &identifier);
    ~IdentifierExpression() override = default;
    ExpressionClass getClass() override;
};
