#pragma once

#include "../Statement.h"
#include "../Expression.h"

class WhileStatement : public Statement {
public:
    Expression *condition;
    Statement *body;

    WhileStatement(int lineNumber, int columnNumber, Expression *condition, Statement *body);
    ~WhileStatement() override;
    StatementClass getClass() override;
};
