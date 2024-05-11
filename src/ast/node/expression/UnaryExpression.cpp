#include "UnaryExpression.h"

#include <iostream>

UnaryExpression::UnaryExpression(UnaryOperator unaryOperator, Expression *operand) : unaryOperator(unaryOperator), operand(operand) {}

UnaryExpression::~UnaryExpression() {
    delete operand;
}

ExpressionClass UnaryExpression::getClass() {
    return ExpressionClass::UNARY_EXPRESSION;
}

Expression *UnaryExpression::clone() {
    return new UnaryExpression(unaryOperator, operand->clone());
}
