#include "TernaryExpression.h"

#include <iostream>

TernaryExpression::TernaryExpression(TernaryOperator ternaryOperator, Expression *leftOperand, Expression *middleOperand, Expression *rightOperand) : ternaryOperator(ternaryOperator), leftOperand(leftOperand), middleOperand(middleOperand), rightOperand(rightOperand) {}

TernaryExpression::~TernaryExpression() {
    delete leftOperand;
    delete middleOperand;
    delete rightOperand;
}

ExpressionClass TernaryExpression::getClass() {
    return ExpressionClass::TERNARY_EXPRESSION;
}

Expression *TernaryExpression::clone() {
    return new TernaryExpression(ternaryOperator, leftOperand->clone(), middleOperand->clone(), rightOperand->clone());
}
