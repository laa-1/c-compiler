#pragma once


#include "../Statement.h"
#include "../Expression.h"

class DefaultStatement : public Statement {
public:
    Statement *statement;

    DefaultStatement(int lineNumber, int columnNumber, Statement *statement);
    ~DefaultStatement() override;
    StatementClass getClass() override;
};
