#include "IdentifierExpression.h"

#include <utility>

IdentifierExpression::IdentifierExpression(int lineNumber, int columnNumber, const std::string &identifier) : Expression(lineNumber, columnNumber), identifier(identifier) {}

ExpressionClass IdentifierExpression::getClass() {
    return ExpressionClass::IDENTIFIER_EXPRESSION;
}

