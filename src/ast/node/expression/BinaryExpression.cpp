#include "BinaryExpression.h"

#include <iostream>

BinaryExpression::BinaryExpression(int lineNumber, int columnNumber, BinaryOperator binaryOperator, Expression *leftOperand, Expression *rightOperand) : Expression(lineNumber, columnNumber), binaryOperator(binaryOperator), leftOperand(leftOperand), rightOperand(rightOperand) {}

BinaryExpression::~BinaryExpression() {
    delete leftOperand;
    delete rightOperand;
}

ExpressionClass BinaryExpression::getClass() {
    return ExpressionClass::BINARY_EXPRESSION;
}


