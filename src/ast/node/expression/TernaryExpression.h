#pragma once

#include "../Expression.h"
#include <memory>


class TernaryExpression : public Expression {
public:
    TernaryOperator ternaryOperator;
    Expression *leftOperand;
    Expression *middleOperand;
    Expression *rightOperand;

    TernaryExpression(int lineNumber, int columnNumber, TernaryOperator ternaryOperator, Expression *leftOperand, Expression *middleOperand, Expression *rightOperand) : Expression(lineNumber, columnNumber), ternaryOperator(ternaryOperator), leftOperand(leftOperand), middleOperand(middleOperand), rightOperand(rightOperand) {}

    ~TernaryExpression() override {
        delete leftOperand;
        delete middleOperand;
        delete rightOperand;
    }

    ExpressionClass getClass() override {
        return ExpressionClass::TERNARY_EXPRESSION;
    }

};
