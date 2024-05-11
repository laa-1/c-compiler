#include "BinaryExpression.h"

#include <iostream>

BinaryExpression::BinaryExpression(BinaryOperator binaryOperator, Expression *leftOperand, Expression *rightOperand) : binaryOperator(binaryOperator), leftOperand(leftOperand), rightOperand(rightOperand) {}

BinaryExpression::~BinaryExpression() {
    delete leftOperand;
    delete rightOperand;
}

ExpressionClass BinaryExpression::getClass() {
    return ExpressionClass::BINARY_EXPRESSION;
}

Expression *BinaryExpression::clone() {
    return new BinaryExpression(binaryOperator, leftOperand->clone(), rightOperand->clone());
}

