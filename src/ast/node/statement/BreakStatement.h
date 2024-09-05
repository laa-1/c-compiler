#pragma once

#include "../Statement.h"

class BreakStatement : public Statement {
public:
    BreakStatement(int lineNumber, int columnNumber);
    ~BreakStatement() override = default;
    StatementClass getClass() override;
};
