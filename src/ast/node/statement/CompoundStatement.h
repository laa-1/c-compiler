#pragma once

#include "../Statement.h"
#include <vector>

class CompoundStatement : public Statement {
public:
    std::vector<Statement *> statementList;

    explicit CompoundStatement(const std::vector<Statement *> &statementList);
    ~CompoundStatement() override;
    StatementClass getClass() override;
};
