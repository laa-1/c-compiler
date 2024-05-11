#pragma once

#include "../Expression.h"
#include "../Type.h"
#include <memory>

class CastExpression : public Expression {
public:
    Type *targetType;
    Expression *operand;

    CastExpression(Type *targetType, Expression *operand);
    ~CastExpression() override;
    ExpressionClass getClass() override;
    Expression *clone() override;
};
