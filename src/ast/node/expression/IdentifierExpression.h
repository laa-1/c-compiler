#pragma once

#include "../Expression.h"

class IdentifierExpression : public Expression {
public:
    std::string identifier;

    IdentifierExpression(int lineNumber, int columnNumber, const std::string &identifier) : Expression(lineNumber, columnNumber), identifier(identifier) {}

    ~IdentifierExpression() override = default;

    ExpressionClass getClass() override {
        return ExpressionClass::IDENTIFIER_EXPRESSION;
    }
};
