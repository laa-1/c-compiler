#pragma once

#include "../Statement.h"
#include "../Expression.h"

class ExpressionStatement : public Statement {
public:
    Expression *expression; // 可以为空

    explicit ExpressionStatement(Expression *expression);
    ~ExpressionStatement() override;
    StatementClass getClass() override;
};
