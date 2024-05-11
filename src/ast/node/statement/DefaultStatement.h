#pragma once


#include "../Statement.h"
#include "../Expression.h"

class DefaultStatement : public Statement {
public:
    Statement *statement;

    explicit DefaultStatement(Statement *statement);
    ~DefaultStatement() override;
    StatementClass getClass() override;
};
