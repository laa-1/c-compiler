#pragma once

#include "../Expression.h"
#include <memory>

class UnaryExpression : public Expression {
public:
    UnaryOperator unaryOperator;
    Expression *operand;

    UnaryExpression(UnaryOperator unaryOperator, Expression *operand);
    ~UnaryExpression() override;
    ExpressionClass getClass() override;
    Expression *clone() override;
};
