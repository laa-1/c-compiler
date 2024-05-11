#pragma once

#include "../Statement.h"
#include "../Expression.h"

class SwitchStatement : public Statement {
public:
    Expression *expression;
    Statement *body;

    SwitchStatement(Expression *expression, Statement *body);
    ~SwitchStatement() override;
    StatementClass getClass() override;
};
