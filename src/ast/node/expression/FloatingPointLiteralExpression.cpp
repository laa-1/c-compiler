#include "FloatingPointLiteralExpression.h"

FloatingPointLiteralExpression::FloatingPointLiteralExpression(double value) : value(value) {}

ExpressionClass FloatingPointLiteralExpression::getClass() {
    return ExpressionClass::FLOAT_LITERAL_EXPRESSION;
}

Expression *FloatingPointLiteralExpression::clone() {
    return new FloatingPointLiteralExpression(value);
}
