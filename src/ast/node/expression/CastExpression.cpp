#include "CastExpression.h"

#include <iostream>

CastExpression::CastExpression(Type *targetType, Expression *operand) : targetType(targetType), operand(operand) {}

CastExpression::~CastExpression() {
    delete targetType;
    delete operand;
}

ExpressionClass CastExpression::getClass() {
    return ExpressionClass::CAST_EXPRESSION;
}

Expression *CastExpression::clone() {
    return new CastExpression(targetType->clone(), operand->clone());
}
