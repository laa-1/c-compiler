#pragma once

#include "../Expression.h"
#include <memory>

class BinaryExpression : public Expression {
public:
    BinaryOperator binaryOperator;
    Expression *leftOperand;
    Expression *rightOperand;

    BinaryExpression(int lineNumber, int columnNumber, BinaryOperator binaryOperator, Expression *leftOperand, Expression *rightOperand) : Expression(lineNumber, columnNumber), binaryOperator(binaryOperator), leftOperand(leftOperand), rightOperand(rightOperand) {}

    ~BinaryExpression() override {
        delete leftOperand;
        delete rightOperand;
    }

    ExpressionClass getClass() override {
        return ExpressionClass::BINARY_EXPRESSION;
    }
};
