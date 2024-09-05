#include "CharacterLiteralExpression.h"

CharacterLiteralExpression::CharacterLiteralExpression(int lineNumber, int columnNumber, char value) : Expression(lineNumber, columnNumber), value(value) {}

ExpressionClass CharacterLiteralExpression::getClass() {
    return ExpressionClass::CHAR_LITERAL_EXPRESSION;
}


