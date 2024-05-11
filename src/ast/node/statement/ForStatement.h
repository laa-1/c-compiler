#pragma once

#include <vector>
#include "../Statement.h"
#include "../Expression.h"
#include "../Declaration.h"

class ForStatement : public Statement {
public:
    std::vector<Declaration *> declarationList;
    Expression *init; // 可以为空
    Expression *condition; // 可以为空，即死循环
    Expression *update; // 可以为空
    Statement *body;

    ForStatement(const std::vector<Declaration *> &initDeclarationList, Expression *initExpression, Expression *condition, Expression *updateExpression, Statement *body);
    ~ForStatement() override;
    StatementClass getClass() override;
};
