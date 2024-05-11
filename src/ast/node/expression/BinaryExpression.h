#pragma once

#include "../Expression.h"
#include <memory>

class BinaryExpression : public Expression {
public:
    BinaryOperator binaryOperator;
    Expression *leftOperand;
    Expression *rightOperand;

    BinaryExpression(BinaryOperator binaryOperator, Expression *leftOperand, Expression *rightOperand);
    ~BinaryExpression() override;
    ExpressionClass getClass() override;
    Expression *clone() override;
};
