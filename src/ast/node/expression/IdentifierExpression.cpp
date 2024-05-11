#include "IdentifierExpression.h"

#include <utility>

IdentifierExpression::IdentifierExpression(std::string identifier) : identifier(std::move(identifier)) {}

ExpressionClass IdentifierExpression::getClass() {
    return ExpressionClass::IDENTIFIER_EXPRESSION;
}

Expression *IdentifierExpression::clone() {
    return new IdentifierExpression(identifier);
}
