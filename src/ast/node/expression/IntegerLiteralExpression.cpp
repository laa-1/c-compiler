#include "IntegerLiteralExpression.h"

IntegerLiteralExpression::IntegerLiteralExpression(int lineNumber, int columnNumber, int value) : Expression(lineNumber, columnNumber), value(value) {}

ExpressionClass IntegerLiteralExpression::getClass() {
    return ExpressionClass::INT_LITERAL_EXPRESSION;
}

