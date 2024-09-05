#include "ExpressionStatement.h"

#include <iostream>

ExpressionStatement::ExpressionStatement(int lineNumber, int columnNumber, Expression *expression) : Statement(lineNumber, columnNumber), expression(expression) {}

ExpressionStatement::~ExpressionStatement() {
    delete expression;
}

StatementClass ExpressionStatement::getClass() {
    return StatementClass::EXPRESSION_STATEMENT;
}