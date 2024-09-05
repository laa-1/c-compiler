#pragma once

#include "../Expression.h"
#include <memory>

class BinaryExpression : public Expression {
public:
    BinaryOperator binaryOperator;
    Expression *leftOperand;
    Expression *rightOperand;

    BinaryExpression(int lineNumber, int columnNumber, BinaryOperator binaryOperator, Expression *leftOperand, Expression *rightOperand);
    ~BinaryExpression() override;
    ExpressionClass getClass() override;
};
