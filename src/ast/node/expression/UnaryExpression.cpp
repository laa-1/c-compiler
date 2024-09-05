#include "UnaryExpression.h"

#include <iostream>

UnaryExpression::UnaryExpression(int lineNumber, int columnNumber, UnaryOperator unaryOperator, Expression *operand) : Expression(lineNumber, columnNumber), unaryOperator(unaryOperator), operand(operand) {}

UnaryExpression::~UnaryExpression() {
    delete operand;
}

ExpressionClass UnaryExpression::getClass() {
    return ExpressionClass::UNARY_EXPRESSION;
}

