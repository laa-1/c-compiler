#include "CharacterLiteralExpression.h"

CharacterLiteralExpression::CharacterLiteralExpression(char value) : value(value) {}

ExpressionClass CharacterLiteralExpression::getClass() {
    return ExpressionClass::CHAR_LITERAL_EXPRESSION;
}

Expression *CharacterLiteralExpression::clone() {
    return new CharacterLiteralExpression(value);
}
