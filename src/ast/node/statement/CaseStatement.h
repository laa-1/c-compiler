#pragma once

#include "../Statement.h"
#include "../Expression.h"

class CaseStatement : public Statement {
public:
    int value;
    Statement *statement;

    CaseStatement(int lineNumber, int columnNumber, int value, Statement *statement) : Statement(lineNumber, columnNumber), value(value), statement(statement) {}

    ~CaseStatement() override {
        delete statement;
    }

    StatementClass getClass() override {
        return StatementClass::CASE_STATEMENT;
    }

};
