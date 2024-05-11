#include "IntegerLiteralExpression.h"

IntegerLiteralExpression::IntegerLiteralExpression(int value) : value(value) {}

ExpressionClass IntegerLiteralExpression::getClass() {
    return ExpressionClass::INT_LITERAL_EXPRESSION;
}

Expression *IntegerLiteralExpression::clone() {
    return new IntegerLiteralExpression(value);
}
