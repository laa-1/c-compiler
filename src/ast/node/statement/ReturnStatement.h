#pragma once

#include "../Statement.h"
#include "../Expression.h"

class ReturnStatement : public Statement {
public:
    Expression *value; // 可以为空

    ReturnStatement(int lineNumber, int columnNumber, Expression *value) : Statement(lineNumber, columnNumber), value(value) {}

    ~ReturnStatement() override {
        delete value;
    }

    StatementClass getClass() override {
        return StatementClass::RETURN_STATEMENT;
    }
};
