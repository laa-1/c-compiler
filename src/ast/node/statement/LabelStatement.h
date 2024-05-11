#pragma once

#include "../Statement.h"

class LabelStatement : public Statement {
public:
    std::string identifier;
    Statement *statement;

    LabelStatement(std::string identifier, Statement *statement);
    ~LabelStatement() override;
    StatementClass getClass() override;
};
