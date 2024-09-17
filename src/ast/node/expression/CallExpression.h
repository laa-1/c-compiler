#pragma once

#include "../Expression.h"
#include <vector>
#include <memory>

class UnaryExpression;

class CallExpression : public Expression {
public:
    Expression *functionAddress;
    std::vector<Expression *> argumentList;

    CallExpression(int lineNumber, int columnNumber, Expression *functionAddress, const std::vector<Expression *> &argumentList) : Expression(lineNumber, columnNumber), functionAddress(functionAddress), argumentList(argumentList) {}

    ~CallExpression() override {
        for (Expression *argument : argumentList) {
            delete argument;
        }
    }

    ExpressionClass getClass() override {
        return ExpressionClass::CALL_EXPRESSION;
    }
};
