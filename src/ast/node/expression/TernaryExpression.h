#pragma once

#include "../Expression.h"
#include <memory>


class TernaryExpression : public Expression {
public:
    TernaryOperator ternaryOperator;
    Expression *leftOperand;
    Expression *middleOperand;
    Expression *rightOperand;

    TernaryExpression(int lineNumber, int columnNumber, TernaryOperator ternaryOperator, Expression *leftOperand, Expression *middleOperand, Expression *rightOperand);
    ~TernaryExpression() override;
    ExpressionClass getClass() override;
};
