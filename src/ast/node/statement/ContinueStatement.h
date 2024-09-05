#pragma once

#include "../Statement.h"

class ContinueStatement : public Statement {
public:
    ContinueStatement(int lineNumber, int columnNumber);
    ~ContinueStatement() override = default;
    StatementClass getClass() override;
};
