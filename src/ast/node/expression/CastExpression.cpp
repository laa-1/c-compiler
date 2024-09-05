#include "CastExpression.h"

#include <iostream>

CastExpression::CastExpression(int lineNumber, int columnNumber, Type *targetType, Expression *operand) : Expression(lineNumber, columnNumber), targetType(targetType), operand(operand) {}

CastExpression::~CastExpression() {
    delete targetType;
    delete operand;
}

ExpressionClass CastExpression::getClass() {
    return ExpressionClass::CAST_EXPRESSION;
}
