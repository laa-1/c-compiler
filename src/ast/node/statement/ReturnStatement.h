#pragma once

#include "../Statement.h"
#include "../Expression.h"

class ReturnStatement : public Statement {
public:
    Expression *value; // 可以为空

    explicit ReturnStatement(Expression *value);
    ~ReturnStatement() override;
    StatementClass getClass() override;
};
