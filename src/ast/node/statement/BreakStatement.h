#pragma once

#include "../Statement.h"

class BreakStatement : public Statement {
public:
    ~BreakStatement() override = default;
    StatementClass getClass() override;
};
