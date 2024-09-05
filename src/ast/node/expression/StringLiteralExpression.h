#pragma once

#include "../Expression.h"
#include "../vector"

class StringLiteralExpression : public Expression {
public:
    std::string value;

    StringLiteralExpression(int lineNumber, int columnNumber, const std::string &value);
    ~StringLiteralExpression() override = default;
    ExpressionClass getClass() override;
};
