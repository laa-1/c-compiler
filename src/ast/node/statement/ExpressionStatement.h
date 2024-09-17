#pragma once

#include "../Statement.h"
#include "../Expression.h"

class ExpressionStatement : public Statement {
public:
    Expression *expression; // 可以为空

    ExpressionStatement(int lineNumber, int columnNumber, Expression *expression) : Statement(lineNumber, columnNumber), expression(expression) {}

    ~ExpressionStatement() override {
        delete expression;
    }

    StatementClass getClass() override {
        return StatementClass::EXPRESSION_STATEMENT;
    }
};
