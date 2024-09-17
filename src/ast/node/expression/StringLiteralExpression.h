#pragma once

#include "../Expression.h"
#include "../vector"

class StringLiteralExpression : public Expression {
public:
    std::string value;

    StringLiteralExpression(int lineNumber, int columnNumber, const std::string &value) : Expression(lineNumber, columnNumber), value(value) {}

    ~StringLiteralExpression() override = default;

    ExpressionClass getClass() override {
        return ExpressionClass::STRING_LITERAL_EXPRESSION;
    }

};
