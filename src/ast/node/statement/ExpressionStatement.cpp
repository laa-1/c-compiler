#include "ExpressionStatement.h"

#include <iostream>

ExpressionStatement::ExpressionStatement(Expression *expression) : expression(expression) {}

ExpressionStatement::~ExpressionStatement() {
    delete expression;
}

StatementClass ExpressionStatement::getClass() {
    return StatementClass::EXPRESSION_STATEMENT;
}
