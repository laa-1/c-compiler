#pragma once

#include <cstdint>
#include "../Expression.h"

class CharacterLiteralExpression : public Expression {
public:
    char value;

    explicit CharacterLiteralExpression(char value);
    ~CharacterLiteralExpression() override = default;
    ExpressionClass getClass() override;
    Expression *clone() override;
};
