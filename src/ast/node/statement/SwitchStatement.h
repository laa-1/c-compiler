#pragma once

#include "../Statement.h"
#include "../Expression.h"

class SwitchStatement : public Statement {
public:
    Expression *expression;
    Statement *body;

    SwitchStatement(int lineNumber, int columnNumber, Expression *expression, Statement *body) : Statement(lineNumber, columnNumber), expression(expression), body(body) {}

    ~SwitchStatement() override {
        delete expression;
        delete body;
    }

    StatementClass getClass() override {
        return StatementClass::SWITCH_STATEMENT;
    }
};
