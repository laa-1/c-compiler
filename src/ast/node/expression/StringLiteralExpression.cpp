#include "StringLiteralExpression.h"

#include <utility>

StringLiteralExpression::StringLiteralExpression(std::string value) : value(std::move(value)) {}

ExpressionClass StringLiteralExpression::getClass() {
    return ExpressionClass::STRING_LITERAL_EXPRESSION;
}

Expression *StringLiteralExpression::clone() {
    return new StringLiteralExpression(value);
}


