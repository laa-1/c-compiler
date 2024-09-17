#pragma once

#include "../Statement.h"
#include "../Expression.h"

class WhileStatement : public Statement {
public:
    Expression *condition;
    Statement *body;

    WhileStatement(int lineNumber, int columnNumber, Expression *condition, Statement *body) : Statement(lineNumber, columnNumber), condition(condition), body(body) {}

    ~WhileStatement() override {
        delete condition;
        delete body;
    }

    StatementClass getClass() override {
        return StatementClass::WHILE_STATEMENT;
    }
};
