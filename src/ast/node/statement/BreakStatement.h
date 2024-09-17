#pragma once

#include "../Statement.h"

class BreakStatement : public Statement {
public:
    BreakStatement(int lineNumber, int columnNumber) : Statement(lineNumber, columnNumber) {}

    ~BreakStatement() override = default;

    StatementClass getClass() override {
        return StatementClass::BREAK_STATEMENT;
    }
};
