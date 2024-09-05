#pragma once

#include "../Statement.h"
#include "../Expression.h"

class IfStatement : public Statement {
public:
    Expression *condition;
    Statement *trueBody;
    Statement *falseBody; // 可以为空

    IfStatement(int lineNumber, int columnNumber, Expression *condition, Statement *trueBody, Statement *falseBody);
    ~IfStatement() override;
    StatementClass getClass() override;
};
