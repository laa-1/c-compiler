#pragma once

#include "../Statement.h"
#include "../Expression.h"

class DoWhileStatement : public Statement {
public:
    Statement *body;
    Expression *condition;

    DoWhileStatement(int lineNumber, int columnNumber, Statement *body, Expression *condition) : Statement(lineNumber, columnNumber), body(body), condition(condition) {}

    ~DoWhileStatement() override {
        delete body;
        delete condition;
    }

    StatementClass getClass() override {
        return StatementClass::DO_WHILE_STATEMENT;
    }
};
