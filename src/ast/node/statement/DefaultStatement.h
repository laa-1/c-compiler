#pragma once


#include "../Statement.h"
#include "../Expression.h"

class DefaultStatement : public Statement {
public:
    Statement *statement;

    DefaultStatement(int lineNumber, int columnNumber, Statement *statement) : Statement(lineNumber, columnNumber), statement(statement) {}

    ~DefaultStatement() override {
        delete statement;
    }

    StatementClass getClass() override {
        return StatementClass::DEFAULT_STATEMENT;
    }
};
