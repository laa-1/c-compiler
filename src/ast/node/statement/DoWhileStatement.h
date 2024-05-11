#pragma once

#include "../Statement.h"
#include "../Expression.h"

class DoWhileStatement : public Statement {
public:
    Statement *body;
    Expression *condition;

    DoWhileStatement(Statement *body, Expression *condition);
    ~DoWhileStatement() override;
    StatementClass getClass() override;
};
