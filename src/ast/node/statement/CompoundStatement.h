#pragma once

#include "../Statement.h"
#include <vector>

class CompoundStatement : public Statement {
public:
    std::vector<Statement *> statementList;

    CompoundStatement(int lineNumber, int columnNumber, const std::vector<Statement *> &statementList);
    ~CompoundStatement() override;
    StatementClass getClass() override;
};
