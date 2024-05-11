#include "CallExpression.h"

#include <iostream>

CallExpression::CallExpression(Expression *functionAddress, const std::vector<Expression *> &argumentList) : functionAddress(functionAddress), argumentList(argumentList) {}

CallExpression::~CallExpression() {
    for (Expression *argument : argumentList) {
        delete argument;
    }
}

ExpressionClass CallExpression::getClass() {
    return ExpressionClass::CALL_EXPRESSION;
}

Expression *CallExpression::clone() {
    std::vector<Expression *> newArgumentList;
    newArgumentList.reserve(argumentList.size());
    for (auto argument : argumentList) {
        newArgumentList.push_back(argument->clone());
    }
    return new CallExpression(functionAddress->clone(), argumentList);
}
