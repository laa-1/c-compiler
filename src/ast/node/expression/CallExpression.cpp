#include "CallExpression.h"

#include <iostream>

CallExpression::CallExpression(int lineNumber, int columnNumber, Expression *functionAddress, const std::vector<Expression *> &argumentList) : Expression(lineNumber, columnNumber), functionAddress(functionAddress), argumentList(argumentList) {}

CallExpression::~CallExpression() {
    for (Expression *argument : argumentList) {
        delete argument;
    }
}

ExpressionClass CallExpression::getClass() {
    return ExpressionClass::CALL_EXPRESSION;
}

