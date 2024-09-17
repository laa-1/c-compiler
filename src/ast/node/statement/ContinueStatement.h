#pragma once

#include "../Statement.h"

class ContinueStatement : public Statement {
public:
    ContinueStatement(int lineNumber, int columnNumber) : Statement(lineNumber, columnNumber) {}

    ~ContinueStatement() override = default;

    StatementClass getClass() override {
        return StatementClass::CONTINUE_STATEMENT;
    }
};
