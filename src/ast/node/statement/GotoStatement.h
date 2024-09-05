#pragma once

#include "../Statement.h"

class GotoStatement : public Statement {
public:
    std::string identifier;

    GotoStatement(int lineNumber, int columnNumber, const std::string &identifier);
    ~GotoStatement() override = default;
    StatementClass getClass() override;
};
