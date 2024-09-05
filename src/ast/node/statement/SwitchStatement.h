#pragma once

#include "../Statement.h"
#include "../Expression.h"

class SwitchStatement : public Statement {
public:
    Expression *expression;
    Statement *body;

    SwitchStatement(int lineNumber, int columnNumber, Expression *expression, Statement *body);
    ~SwitchStatement() override;
    StatementClass getClass() override;
};
