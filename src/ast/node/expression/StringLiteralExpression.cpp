#include "StringLiteralExpression.h"

#include <utility>

StringLiteralExpression::StringLiteralExpression(int lineNumber, int columnNumber, const std::string &value) : Expression(lineNumber, columnNumber), value(value) {}

ExpressionClass StringLiteralExpression::getClass() {
    return ExpressionClass::STRING_LITERAL_EXPRESSION;
}


