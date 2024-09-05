#include "FloatingPointLiteralExpression.h"

FloatingPointLiteralExpression::FloatingPointLiteralExpression(int lineNumber, int columnNumber, double value) : Expression(lineNumber, columnNumber), value(value) {}

ExpressionClass FloatingPointLiteralExpression::getClass() {
    return ExpressionClass::FLOAT_LITERAL_EXPRESSION;
}

