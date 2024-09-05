#pragma once

#include "../Statement.h"
#include "../Expression.h"

class DoWhileStatement : public Statement {
public:
    Statement *body;
    Expression *condition;

    DoWhileStatement(int lineNumber, int columnNumber, Statement *body, Expression *condition);
    ~DoWhileStatement() override;
    StatementClass getClass() override;
};
