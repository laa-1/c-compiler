#pragma once

#include <cstdint>
#include "../Expression.h"

class CharacterLiteralExpression : public Expression {
public:
    char value;

    CharacterLiteralExpression(int lineNumber, int columnNumber, char value);
    ~CharacterLiteralExpression() override = default;
    ExpressionClass getClass() override;
};
