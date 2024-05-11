#pragma once

#include "../Expression.h"
#include <vector>
#include <memory>

class UnaryExpression;

class CallExpression : public Expression {
public:
    Expression *functionAddress;
    std::vector<Expression *> argumentList;

    CallExpression(Expression *functionAddress, const std::vector<Expression *> &argumentList);
    ~CallExpression() override;
    ExpressionClass getClass() override;
    Expression *clone() override;
};
