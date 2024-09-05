#include "TernaryExpression.h"

#include <iostream>

TernaryExpression::TernaryExpression(int lineNumber, int columnNumber, TernaryOperator ternaryOperator, Expression *leftOperand, Expression *middleOperand, Expression *rightOperand) : Expression(lineNumber, columnNumber), ternaryOperator(ternaryOperator), leftOperand(leftOperand), middleOperand(middleOperand), rightOperand(rightOperand) {}

TernaryExpression::~TernaryExpression() {
    delete leftOperand;
    delete middleOperand;
    delete rightOperand;
}

ExpressionClass TernaryExpression::getClass() {
    return ExpressionClass::TERNARY_EXPRESSION;
}


