#pragma once

#include <cstdint>
#include "../Expression.h"

class CharacterLiteralExpression : public Expression {
public:
    char value;

    CharacterLiteralExpression(int lineNumber, int columnNumber, char value) : Expression(lineNumber, columnNumber), value(value) {}

    ~CharacterLiteralExpression() override = default;

    ExpressionClass getClass() override {
        return ExpressionClass::CHAR_LITERAL_EXPRESSION;
    }
};
