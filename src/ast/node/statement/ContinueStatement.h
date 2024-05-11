#pragma once

#include "../Statement.h"

class ContinueStatement : public Statement {
public:
    ~ContinueStatement() override = default;
    StatementClass getClass() override;
};
