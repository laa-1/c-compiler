#pragma once

#include "../Statement.h"

class GotoStatement : public Statement {
public:
    std::string identifier;

    explicit GotoStatement(std::string identifier);
    ~GotoStatement() override = default;
    StatementClass getClass() override;
};
